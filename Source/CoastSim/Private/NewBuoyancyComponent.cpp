// Fill out your copyright notice in the Description page of Project Settings.


#include "NewBuoyancyComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UNewBuoyancyComponent::UNewBuoyancyComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	ParentActor = GetOwner();
}


// Called when the game starts
void UNewBuoyancyComponent::BeginPlay()
{
	Super::BeginPlay();
	ActorTransform = ParentActor->GetActorTransform();
	WorldActorLocation = ParentActor->GetActorLocation();
	WorldActorRotation = ParentActor->GetActorRotation();
	if (MyStaticMeshComponent->IsValidLowLevelFast())
	{
		RelativeStaticMeshLocation = MyStaticMeshComponent->GetRelativeLocation();
		RelativeStaticMeshRotation = MyStaticMeshComponent->GetRelativeRotation();
	}
	ParentActor->SetActorLocation(FVector(WorldActorLocation.X, WorldActorLocation.Y, 0));
	FFTCalculator = InitializeWaterZoneReference();
}

// Called every frame
void UNewBuoyancyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bWaterZoneValid)
	{
		if (ActorTransform.GetLocation() != ParentActor->GetActorLocation())
		{
			ActorTransform = ParentActor->GetActorTransform();
			WorldActorLocation = ParentActor->GetActorLocation();
			WorldActorRotation = ParentActor->GetActorRotation();
			ParentActor->SetActorLocation(FVector(WorldActorLocation.X, WorldActorLocation.Y, 0));
		}

		if (PontoonsLocations.Num() > 2)
		{
			const TArray<FVector> BuoyancyArray = GetBuoyancyArray(PontoonsLocations);
			const FQuat ActorQuat = CalculateBuoyancyRotation(BuoyancyArray);
			const FVector BuoyancyLocation = GetMultiBuoyancyLocation(PontoonsLocations);
			const FRotator BuoyancyRotation = ActorQuat.Rotator() * RotationStrength + WorldActorRotation;

			if (MyStaticMeshComponent->IsValidLowLevelFast())
			{
				MyStaticMeshComponent->SetWorldLocation(BuoyancyLocation);
				MyStaticMeshComponent->SetWorldRotation(BuoyancyRotation);
			}

			if (DebugPoints) { DrawBuoyancyArrayDebugPoints(BuoyancyArray); }
		}
		else
		{
			if (MyStaticMeshComponent->IsValidLowLevelFast())
			{
				const FVector BuoyancyLocation = GetBuoyancyLocation(PontoonsLocations[0]);
				MyStaticMeshComponent->SetWorldLocation(BuoyancyLocation);
			}
		}
	}
}


// ?Finds the location of the pontoon points that is dislocated by the FFT water shading
FVector UNewBuoyancyComponent::GetBuoyancyLocation(FVector RelativeLocation)
{
	FVector BuoyancyLocation = FVector::ZeroVector;
	FVector WorldLocation = ActorTransform.TransformPosition(RelativeLocation);
	if (FFTCalculator == nullptr) { return BuoyancyLocation; }
	else {

		FVector GridPointLocation = FVector(WorldLocation.X, WorldLocation.Y, -RelativeLocation.Z) / FFTCalculator->scale * FFTCalculator->multiplyScale;
		FVector Displacement = FFTCalculator->GetDisplacementAtPoint(GridPointLocation);

		BuoyancyLocation = GridPointLocation * FFTCalculator->scale / FFTCalculator->multiplyScale + Displacement / FFTCalculator->scale / FFTCalculator->overlapScale;

		return BuoyancyLocation;
	}
}


// ?Get the avarage location of all pontoons to determine the bouyant location
FVector UNewBuoyancyComponent::GetMultiBuoyancyLocation(TArray<FVector> PontoonsArray)
{
	FVector BuoyancyLocation = FVector::ZeroVector;
	FVector AveragePontoons = FindAverageLocation(PontoonsArray);

	BuoyancyLocation = GetBuoyancyLocation(AveragePontoons);
	return BuoyancyLocation;
}


// Find OceanWaterZone in the scene, save it, and return the FFTCalculator
FOceanFFTCalculator* UNewBuoyancyComponent::InitializeWaterZoneReference()
{
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AOceanWaterZone::StaticClass(), FoundActors);

	if (FoundActors.Num() > 0)
	{
		for (AActor* FoundActor : FoundActors)
		{
			OceanWaterZone = Cast<AOceanWaterZone>(FoundActor);
			if (OceanWaterZone)
			{
				return &OceanWaterZone->FFTCalculator;
			}
		}
	}
	bWaterZoneValid = false;
	UE_LOG(LogTemp, Warning, TEXT("WaterZone isn't valid"));
	return nullptr;
}


// Find avarage world location on all points in input array
FVector UNewBuoyancyComponent::FindAverageLocation(TArray<FVector> Locations)
{
	FVector AverageLocation = FVector::ZeroVector;
	for (FVector& Location : Locations)
	{
		AverageLocation = AverageLocation + Location;
	}
	AverageLocation = AverageLocation / Locations.Num();
	return AverageLocation;
}


// 
TArray<FVector> UNewBuoyancyComponent::GetBuoyancyArray(TArray<FVector> Points)
{
	TArray<FVector> PointArray = {};
	for (FVector& Point : Points)
	{
		PointArray.Add(GetBuoyancyLocation(Point));
	}
	return PointArray;
}


// Lerps the different rotations calculated by the CalculateWaveRotation() based on the distance to the parent actor (blueprint root)
FQuat UNewBuoyancyComponent::CalculateBuoyancyRotation(const TArray<FVector> Points)
{
	FQuat AverageRotation = FQuat::Identity;

	for (const FVector& WavePoint : Points)
	{
		float DistanceToCenter = FVector::Dist(ParentActor->GetActorLocation(), WavePoint);
		float Weight = 1.0f / (DistanceToCenter + SMALL_NUMBER);

		FQuat WaveRotation = CalculateWaveRotation(WavePoint);

		// Accumulate the weighted rotation
		AverageRotation = FQuat::Slerp(AverageRotation, WaveRotation, Weight);
	}
	return AverageRotation;
}


// Calculates the angle from the parent actor location (blueprint root) to the WavePoint relative to the z-axis
FQuat UNewBuoyancyComponent::CalculateWaveRotation(const FVector& WavePoint)
{
	FVector TargetVector(0.0f, 0.0f, 1.0f); // z-axis
	FVector WaveDirection = (WavePoint - ParentActor->GetActorLocation()).GetSafeNormal();
	FQuat WaveRotation = FQuat::FindBetween(WaveDirection, TargetVector);

	return WaveRotation;
}


// Draw translated pontoon placements to the scene
void UNewBuoyancyComponent::DrawBuoyancyArrayDebugPoints(const TArray<FVector>& BuoyancyArray)
{
	for (const FVector& Point : BuoyancyArray)
	{
		DrawDebugPoint(GetWorld(), Point, 50.f, FColor(255.f, 0.f, 0.f, 255.f), false, 0.f, 0);
	}
}