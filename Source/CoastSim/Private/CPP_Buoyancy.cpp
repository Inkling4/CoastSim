// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Buoyancy.h"

#include <gsl/pointers>

// Sets default values for this component's properties
UCPP_Buoyancy::UCPP_Buoyancy()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCPP_Buoyancy::BeginPlay()
{
	Super::BeginPlay();

	// ...

	OwnerActor = GetOwner();
	USceneComponent* rootComponent = OwnerActor->GetRootComponent();
	
	//VisualComponent = Cast<USceneComponent>(OwnerActor->GetDefaultSubobjectByName(TEXT("Visual"))); if (VisualComponent == nullptr) UE_LOG(LogTemp, Error, TEXT("No Visual Component on buoyancy object"))
}


// Called every frame
void UCPP_Buoyancy::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

		SurfaceAveragePos();
	
}

float UCPP_Buoyancy::SurfaceAveragePos()
{
	AActor* owner = GetOwner();

	
	TArray<FHitResult> HitResults;
	FVector RayOrigin = owner->GetActorLocation();
	float RayDistance = 1000.0f;
	FVector RayEnd = RayOrigin + FVector(0, 0, RayDistance);

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(owner); // Ignore own actor

	//bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResults, RayOrigin, RayEnd, ECC_GameTraceChannel1, CollisionParams);
	bool bHit = GetWorld()->LineTraceMultiByChannel(HitResults, RayOrigin, RayEnd, ECC_WorldStatic, CollisionParams);

	DrawDebugLine(GetWorld(), RayOrigin, RayEnd, FColor::Yellow, false, -1, 0, 1);

	//FString Msg = FString::Printf(TEXT("%f %f %f"), RayOrigin.X, RayOrigin.Y, RayOrigin.Z);	GEngine->AddOnScreenDebugMessage(-1, -1, FColor::Red, Msg);
	
	if (bHit && VisualComponent != nullptr)
	{
		for (FHitResult Hit : HitResults)
		{
			//UE_LOG(LogTemp, Log, TEXT("Moving component"));
			UE_LOG(LogTemp, Log, TEXT("%f"), Hit.Distance);
			
			FVector wantedPos(0, 0, Hit.Distance);

			// Set floating object to surface position
			VisualComponent->SetRelativeLocation(wantedPos);
			
			//OwnerActor->SetActorLocation(wantedPos);
			if (Hit.GetActor()->GetName() == TEXT("OceanMesh"))
			{
				
			}
		}
	}
	else if (!bHit)
	{
		UE_LOG(LogTemp, Log, TEXT("LineTrace didn't hit anything"));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("No Visual Component"));
	}
	
	return 0;
}

float UCPP_Buoyancy::VisualComponentOffset()
{
	//auto actorLocation = OwnerActor->GetActorLocation().Z;
	return VisualComponent->GetRelativeLocation().Z; 
}

