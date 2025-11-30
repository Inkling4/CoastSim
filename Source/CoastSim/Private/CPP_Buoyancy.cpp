// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_Buoyancy.h"

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
	
	VisualComponent = Cast<UStaticMeshComponent>(GetDefaultSubobjectByName(TEXT("Visual")));
	if (!VisualComponent)
		UE_LOG(LogTemp, Error, TEXT("No Visual Component on buoyancy object"))
}


// Called every frame
void UCPP_Buoyancy::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	if (VisualComponent)
	{
		SurfaceAveragePos();
	}
}

float UCPP_Buoyancy::SurfaceAveragePos()
{
	AActor* owner = GetOwner();

	
	FHitResult HitResult;
	FVector RayOrigin = owner->GetActorLocation();
	float RayDistance = 1000.0f;
	FVector RayEnd = RayOrigin + FVector(0, 0, RayDistance);

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(owner); // Ignore own actor

	bool bHit = GetWorld()->LineTraceSingleByObjectType(HitResult, RayOrigin, RayEnd, ObjectParams, CollisionParams);

	
	if (bHit) 
	{
		FVector wantedPos(0, 0, HitResult.Distance);
		VisualComponent->SetRelativeLocation(wantedPos);

		return HitResult.Distance;
	}
	
	return 0;
}

float UCPP_Buoyancy::VisualComponentOffset()
{
	//auto actorLocation = OwnerActor->GetActorLocation().Z;
	return VisualComponent->GetRelativeLocation().Z; 
}

