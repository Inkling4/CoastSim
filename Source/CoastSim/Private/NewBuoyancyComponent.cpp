
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

	// Initialize variables
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



	// Don't run code unless there is a sufficient number of points
	if (XPoints < 1 || YPoints < 1)
		return;

	// Add PontoonLocations based on length of object and number of wanted points
	PontoonsLocations.Empty();
	for (size_t i = 0; i < YPoints; i++)
	{
		for (size_t j = 0; j < XPoints; j++)
		{
			float yDistanceBetweenPoints = ObjectLengthY / YPoints;
			float yPos = i * yDistanceBetweenPoints - float(YPoints-1)/2 * yDistanceBetweenPoints;

			float xDistanceBetweenPoints = ObjectLengthX / XPoints;
			float xPos = j * xDistanceBetweenPoints - float(XPoints - 1) / 2 * xDistanceBetweenPoints;

			PontoonsLocations.Add(FVector(yPos, xPos, 0));

			UE_LOG(LogTemp, Warning, TEXT("Created PontoonPoint"));
		}
	}

	// Cuts of corners to the point-grid for more rounded objects
	if (CutCorners && XPoints > 2 && YPoints > 2)
	{
		PontoonsLocations.RemoveAt(PontoonsLocations.Num() - 1);
		PontoonsLocations.RemoveAt(PontoonsLocations.Num() - (XPoints-1));
		PontoonsLocations.RemoveAt(XPoints-1);
		PontoonsLocations.RemoveAt(0);
	}
}

// Called every frame
void UNewBuoyancyComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bWaterZoneValid && MyStaticMeshComponent)
	{
		// Set parent z-location to 0
		if (ActorTransform.GetLocation() != ParentActor->GetActorLocation())
		{
			ActorTransform = ParentActor->GetActorTransform();
			WorldActorLocation = ParentActor->GetActorLocation();
			WorldActorRotation = ParentActor->GetActorRotation();
			
			ParentActor->SetActorLocation(FVector(WorldActorLocation.X, WorldActorLocation.Y, 0));
		}

		
		if (PontoonsLocations.Num() > 2) // If you have set any pontoon locations
		{
			BuoyancyArray.Empty();
			BuoyancyArray = GetBuoyancyArray(PontoonsLocations);
			const FQuat ActorQuat = CalculateBuoyancyRotation(BuoyancyArray);
			const FVector BuoyancyLocation = GetMultiBuoyancyLocation(PontoonsLocations);
			const FRotator BuoyancyRotation = FRotator(ActorQuat.Rotator()) + WorldActorRotation; //*RotationStrength
			//const FRotator BuoyancyRotation = FRotator(ActorQuat.Rotator().Pitch, ActorQuat.Rotator().Yaw, 0) * RotationStrength + WorldActorRotation;
						
			/*
			// Debugging
			FString debugMsg = FString::Printf(TEXT("Yaw: %f, Pitch: %f"), float(ActorQuat.Rotator().Yaw), float(ActorQuat.Rotator().Pitch));
			UE_LOG(LogTemp, Warning, TEXT("Yaw: %f"), float(ActorQuat.Rotator().Yaw));
			UE_LOG(LogTemp, Warning, TEXT("Pitch: %f"), float(ActorQuat.Rotator().Pitch));
			//*/

			// Place and rotate the staticMesh object
			if (MyStaticMeshComponent->IsValidLowLevelFast())
			{
				MyStaticMeshComponent->SetWorldLocation(BuoyancyLocation);
				MyStaticMeshComponent->SetWorldRotation(BuoyancyRotation);
			}

			// Debug
			if (DebugPoints) { DrawBuoyancyArrayDebugPoints(BuoyancyArray); }
		}
		else // Set location to the one pontoon you have (default is having one FVector::ZeroVector)
		{
			if (MyStaticMeshComponent->IsValidLowLevelFast())
			{
				const FVector BuoyancyLocation = GetBuoyancyLocation(PontoonsLocations[0]);
				MyStaticMeshComponent->SetWorldLocation(BuoyancyLocation);
			}
		}
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("MISSING CRUCIAL COMPONENTS (StaticMesh or OceanWaterZone)! BUOYANCY SCRIPT WILL NOT RUN!"));
	}
}


// ?Finds the location of the pontoon points that is dislocated by the FFT water shading computing
FVector UNewBuoyancyComponent::GetBuoyancyLocation(FVector RelativeLocation)
{
	// Safe measure
	FVector BuoyancyLocation = FVector::ZeroVector;

	//TODO: Figure out what transformPosition does
	FVector WorldLocation = ActorTransform.TransformPosition(RelativeLocation);
	if (FFTCalculator == nullptr) { return BuoyancyLocation; }
	else {
		
		FVector GridPointLocation = FVector(WorldLocation.X, WorldLocation.Y, -RelativeLocation.Z) / FFTCalculator->scale * FFTCalculator->multiplyScale;
		FVector Displacement = FFTCalculator->GetDisplacementAtPoint(GridPointLocation);

		// Add location and displacement while factoring in the scale that we set to the ocean shader
		BuoyancyLocation = GridPointLocation
								* FFTCalculator->scale / FFTCalculator->multiplyScale
							+ Displacement
								/ FFTCalculator->scale / FFTCalculator->overlapScale;

		return BuoyancyLocation;
	}
}


// ?Get the avarage location of all pontoons to determine the buoyant location
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


// Get buoyancy points in an array
TArray<FVector> UNewBuoyancyComponent::GetBuoyancyArray(TArray<FVector> Points)
{
	TArray<FVector> PointArray = {};
	for (size_t i = 0; i < Points.Num(); i++)
	{
		PointArray.Add(GetBuoyancyLocation(Points[i]));
	}
	return PointArray;
}


// Lerps the different rotations calculated by the CalculateWaveRotation() based on the distance to the parent actor (blueprint root)
FQuat UNewBuoyancyComponent::CalculateBuoyancyRotation(const TArray<FVector> Points)
{
	FQuat AverageRotation = FQuat::Identity;

	/*
	for (const FVector& WavePoint : Points)
	{
		float DistanceToCenter = FVector::Dist(ParentActor->GetActorLocation(), WavePoint);
		float Weight = 1.0f / (DistanceToCenter + SMALL_NUMBER);

		FQuat WaveRotation = CalculateWaveRotation(WavePoint);

		// Accumulate the weighted rotation
		AverageRotation = FQuat::Slerp(AverageRotation, WaveRotation, Weight);
	}

	FVector AvarageVector = FVector(-10000, 0, 0);//FVector::ZeroVector;

	for (const FVector& WavePoint : Points)
	{
		AvarageVector += WavePoint;
	}

	//AvarageVector /= Points.Num();

	// Should turn the whole function into a rotator
	AverageRotation = FQuat(AvarageVector.Rotation());
	*/

	FRotator currentRotation = MyStaticMeshComponent->GetComponentRotation();
	TArray<FVector> boatPoints;
	float pitchRotation = 0;
	int pitchIncrements = 0;
	float rollRotation = 0;
	int rollIncrements = 0;

	// Offset, rotate and add pontoon positions into a new array (foreach loop sometimes doesn't get points in order)
	for (size_t i = 0; i < PontoonsLocations.Num(); i++)
	{
		// Find the world position of the pontoon point based on the StaticMesh position
		FVector staticMeshPoint = currentRotation.RotateVector(PontoonsLocations[i] + MyStaticMeshComponent->GetComponentLocation());

		// Get z offset between the boat point and the equivalent wave point
		float zOffset = 0;
		if (BuoyancyArray.IsValidIndex(i))
			zOffset = (staticMeshPoint - BuoyancyArray[i]).Z / 1;

		// If the point is on the y-axis of the object calculate pitch
		if (PontoonsLocations[i].Y == 0 && PontoonsLocations[i].X != 0) // Pitch
		{
			float a = -zOffset * RotationStrength;
			float b = 1/(PontoonsLocations[i].X);

			pitchRotation += a * b;
			pitchIncrements++;
		}
		// If the point is in any other position
		else if (PontoonsLocations[i].Y != 0) // Roll
		{
			float a = zOffset * RotationStrength;
			float b = 1 / (PontoonsLocations[i].Y);

			rollRotation += a * b;
			rollIncrements++;
		}
	}
	// Debug
	DrawBuoyancyArrayDebugPoints(boatPoints);

	
	if (rollIncrements)
	{
		rollRotation *= 40 / rollIncrements;
		//UE_LOG(LogTemp, Warning, TEXT("Roll: %f"), rollRotation);

		rollRotation = FMath::Lerp(currentRotation.Roll, rollRotation, .5f);
		rollRotation = FMath::Clamp(rollRotation, -20, 20);
	}
	
	if (pitchIncrements)
	{
		pitchRotation *= 40 / pitchIncrements;
		//UE_LOG(LogTemp, Warning, TEXT("Pitch: %f"), pitchRotation);

		pitchRotation = FMath::Lerp(currentRotation.Pitch, pitchRotation, .7f);
		pitchRotation = FMath::Clamp(pitchRotation, -20, 20);

	}
	
	FRotator rotation = FRotator(pitchRotation, 0, rollRotation);

	//TODO: Can change this to rotator
	return FQuat(rotation);
}


// Calculates the angle from the parent actor location (blueprint root) to the WavePoint relative to the z-axis
// NOT IN USE!
FQuat UNewBuoyancyComponent::CalculateWaveRotation(const FVector& WavePoint)
{
	FVector TargetVector(0.0f, 0.0f, 1.0f); // z-axis
	FVector WaveDirection = (WavePoint - ParentActor->GetActorLocation()).GetSafeNormal();
	FQuat WaveRotation = FQuat::FindBetween(WaveDirection, TargetVector);

	return WaveRotation;
}



//TODO: DrawArrayDebugPoints
// Draw translated pontoon placements to the scene
void UNewBuoyancyComponent::DrawBuoyancyArrayDebugPoints(const TArray<FVector>& array)
{
	for (const FVector& Point : array)
	{
		DrawDebugPoint(GetWorld(), Point, 50.f, FColor(255.f, 0.f, 0.f, 255.f), false, 0.f, 0);
	}
}