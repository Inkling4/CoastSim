// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_FlockManager.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "HAL/PlatformTime.h"
#include "Components/InstancedStaticMeshComponent.h"
#include "CPP_BoidHelper.h"
#include "SWarningOrErrorBox.h"
#include "Async/ParallelFor.h"

// Sets default values
ACPP_FlockManager::ACPP_FlockManager() : SpatialHashGrid(1000, true)
{
	//The tick interval for the whole object, adjust to performance
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickInterval = BoidUpdateInterval;

	InstancedMesh = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("InstancedMesh"));
	RootComponent = InstancedMesh;
	
	InstancedMesh->bUseAsOccluder = false;
	InstancedMesh->CastShadow = false;
	InstancedMesh->bDisableCollision = true;

	//Culling distance to set when the birds stop rendereing
	InstancedMesh->InstanceStartCullDistance = 1000.f;
	InstancedMesh->InstanceEndCullDistance = 20000.f;
	
	AlignmentFactor = 2.0f;
	CohesionFactor = 2.0f;
	SeparationFactor = 1.0f;
	MoveToTargetFactor = 3.0f;
	ObstacleAvoidanceFactor = 10.0f;
	
	bShowDebugAvoidance = false;
	bDrawGrid = false;

	BoidUpdateInterval = 0.033f;
	BoidUpdateAccumulator = 0.0f;
}

// Called when the game starts or when spawned
void ACPP_FlockManager::BeginPlay()
{
	Super::BeginPlay();

	if (BoidMesh)
	{
		InstancedMesh->SetStaticMesh(BoidMesh);
	}
	
	FCPP_BoidHelper::Init();
	
	InitializeBoids();
	
	UE_LOG(LogTemp, Warning, TEXT("BeginPlay called. BoidUpdateInterval = %f"), BoidUpdateInterval);
}

void ACPP_FlockManager::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	SpatialHashGrid.ClearGrid();
	Super::EndPlay(EndPlayReason);
}

// Called every frame
void ACPP_FlockManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UE_LOG(LogTemp, Warning, TEXT("DeltaTime: %f"), DeltaTime);

	BoidUpdateAccumulator += DeltaTime;

	//Timer to call UpdateBoids()
	while (BoidUpdateAccumulator >= BoidUpdateInterval)
	{
		UpdateBoids(BoidUpdateInterval);
		BoidUpdateAccumulator -= BoidUpdateInterval;
	}

	float Alpha = BoidUpdateAccumulator / BoidUpdateInterval;

	//Calculate all the new transforms between each UpdateBoid locations
	TArray<FTransform> NewTransforms;
	NewTransforms.SetNum(Boids.Num());
	ParallelFor(Boids.Num(), [this, Alpha,&NewTransforms](int32 i)	
	{
		const FBoid& Boid = Boids[i];
		FVector InterpPos = FMath::Lerp(Boid.PrevPosition, Boid.TargetPosition, Alpha);
	 
		FTransform NewTransform;
		NewTransform.SetLocation(InterpPos);
		NewTransform.SetRotation(Boid.TargetRotation);

		NewTransforms[i] = NewTransform;
	 
	});

	//Apply new transforms to the instance mesh
	for (int32 i = 0; i < NewTransforms.Num(); ++i)
	{
		InstancedMesh->UpdateInstanceTransform(i, NewTransforms[i], true, false);
	}

	InstancedMesh->MarkRenderStateDirty();
	
}

void ACPP_FlockManager::InitializeBoids()
{
	
	Boids.SetNum(NumberOfBoids);

	for (int32 i = 0; i < NumberOfBoids; i++)
	{
		FVector SpawnLocation = GetActorLocation() + FMath::VRand() * 500.f;

		Boids[i] = BoidStruct;

		Boids[i].Position = SpawnLocation;
		Boids[i].Velocity = FMath::VRand() * 500.f;

		FTransform Transform;
		Transform.SetLocation(SpawnLocation);
		InstancedMesh->AddInstance(Transform);
		
		Boids[i].PrevPosition = Boids[i].Position;
		Boids[i].TargetPosition = Boids[i].Position;

		Boids[i].PrevRotation = FQuat(Boids[i].Velocity.Rotation());
		Boids[i].TargetRotation = FQuat(Boids[i].Velocity.Rotation());

		SpatialHashGrid.InsertBoid(i,SpatialHashGrid.GetCellVector(Boids[i].Position));
	}
}

void ACPP_FlockManager::UpdateBoids(float DeltaTime)
{
	//Clears and repopulates the spatial hash grid each update
	for (int32 i = 0; i < Boids.Num(); i++)
	{
		FBoid& Boid = Boids[i];
		FIntVector OldCell = Boid.OldCellLocation;
		FIntVector NewCell = SpatialHashGrid.GetCellVector(Boid.Position);
		
		if (OldCell != NewCell)
		{
			SpatialHashGrid.RemoveBoid(i, OldCell);
			SpatialHashGrid.InsertBoid(i, NewCell);

			if (SpatialHashGrid.CheckIfCellIsEmpty(OldCell)) 
				SpatialHashGrid.DeleteCell(OldCell);
		}
		Boid.OldCellLocation = NewCell;
	}

	//Draws the active grid cells
	if (bDrawGrid)
	{
		SpatialHashGrid.DrawGrid(GetWorld(), FColor::Red, 0.0f);
	}

	BoidsBuffer.SetNumUninitialized(Boids.Num());
	
	ParallelFor(Boids.Num(), [this, DeltaTime](int32 i)
	{
		
		TArray<int32> NeighbourIndecies;
		SpatialHashGrid.GetNeighbourBoids(Boids[i].Position, NeighbourIndecies);
		
		TArrayView<const FBoid> BoidsView(Boids);

		FBoid Boid = BoidsView[i];

		Boid.PrevPosition = Boid.Position;
		Boid.PrevRotation = FQuat(Boid.Velocity.Rotation());

		Boid.MaxSpeed = BoidStruct.MaxSpeed;
		Boid.MinSpeed = BoidStruct.MinSpeed;
		Boid.MaxForce = BoidStruct.MaxForce;

		ApplyFlockingForces(Boid, i, NeighbourIndecies, BoidsView);

		if (IsHeadingForCollision(Boid))
		{
			FVector AvoidDir = SteerTowards(ObstacleRays(Boid),Boid) * ObstacleAvoidanceFactor ;
			
			Boid.Acceleration += AvoidDir;
		}
		
		FVector DesiredAcceleration = Boid.Acceleration.GetClampedToMaxSize(Boid.MaxForce); //Boid.Acceleration.GetSafeNormal() * Boid.MaxForce;
		Boid.Acceleration = FMath::Lerp(Boid.Acceleration, DesiredAcceleration, 0.1f);
		
		//Boid.Velocity += Boid.Acceleration * BoidUpdateInterval;

		FVector NewVelocity = Boid.Velocity + Boid.Acceleration *DeltaTime;
		Boid.Velocity = FMath::Lerp(Boid.Velocity, NewVelocity, 0.3f);
		
		//Boid.Velocity = Boid.Velocity.GetClampedToMaxSize(Boid.MaxSpeed);
		Boid.Velocity = Boid.Velocity.GetClampedToSize(Boid.MinSpeed, Boid.MaxSpeed);
		Boid.Position += Boid.Velocity * DeltaTime;

		
		UE_LOG(LogTemp, Warning, TEXT("X: %.f, Y: %.f, Z: %.f"), Boid.Velocity.X, Boid.Velocity.Y, Boid.Velocity.Z);


		Boid.Acceleration = FVector::ZeroVector;

		Boid.TargetPosition = Boid.Position;
		FQuat DesiredRotation = FQuat(Boid.Velocity.Rotation());
		Boid.TargetRotation = FQuat::Slerp(Boid.TargetRotation, DesiredRotation, 0.2f);

		// Multithreading array
		BoidsBuffer[i] = Boid;
	});

	Boids = BoidsBuffer;
	//DrawDebugSphere(GetWorld(), GetActorLocation(), BoundaryRadius, 32, FColor::Red, false, -1.f, 0, 2.f);
	
}

void ACPP_FlockManager::ApplyFlockingForces(FBoid& Boid, int32 BoidIndex, const TArray<int32>& NeighbourIndecies, const TArrayView<const FBoid> BoidsView) const
{
	
	FVector AlignSum = FVector::ZeroVector;
	FVector CohesionSum = FVector::ZeroVector;
	FVector SeparationSum = FVector::ZeroVector;
	int32 AlignCount = 0, CohesionCount = 0, SeparationCount = 0;

	for (int32 NeighbourIndex : NeighbourIndecies)
	{
		if (NeighbourIndex == BoidIndex) continue;

		const FBoid& NeighborBoid = BoidsView[NeighbourIndex];
		float DistSq = FVector::DistSquared(NeighborBoid.Position, Boid.Position);

		if (DistSq < FMath::Square(AlignThreshold))
		{
			AlignSum += NeighborBoid.Velocity;
			AlignCount++;
		}
		if (DistSq < FMath::Square(CohesionThreshold))
		{
			CohesionSum += (NeighborBoid.Position - Boid.Position).GetSafeNormal() * DistSq / CohesionThreshold;//NeighborBoid.Position;
			CohesionCount++;
		}
		// Maybe only target close birds?
		if (DistSq < FMath::Square(SeparationThreshold) && DistSq > 0)
		{
			SeparationSum += (Boid.Position - NeighborBoid.Position).GetSafeNormal() * SeparationThreshold / (DistSq/1000);//FMath::Sqrt(DistSq);
			SeparationCount++;
		}
	}
	
	/*
	if (HeightLoss != 0)
	{
		Boid.Acceleration += FVector(0, 0, Boid.Velocity.Z * 2*9.81 * (Boid.Velocity.Z > 0 ? -1 : 1)); //TODO: Divide with delta time?
		if ((Boid.Acceleration.Z < SpeedAvoidance && !Boid.Ascending) || (Boid.Acceleration.Z < -SpeedAvoidance && Boid.Ascending))
		{
			Boid.Ascending = true;
			//Boid.Acceleration += SteerTowards(FVector(Boid.Velocity.X, Boid.Velocity.Y, HeightLoss), Boid) * 10;
		}
		else
		{
			Boid.Ascending = false;
		}
		
		
		const TCHAR* BoolText = Boid.Ascending ? TEXT("True") : TEXT("False");
		UE_LOG(LogTemp, Warning, TEXT("Z: %.f, Ascending: %s"), Boid.Acceleration.Z, BoolText);

		//Boid.Acceleration += SteerTowards(Boid.Acceleration, Boid)* 50;
		//Boid.Acceleration.Normalize();
	}*/
		
	if (oldFlocking)
	{
		if (AlignCount > 0) // Steer in the same direction
			Boid.Acceleration += SteerTowards(AlignSum / AlignCount, Boid) * AlignmentFactor;
		if (CohesionCount > 0) // Stay together
			Boid.Acceleration += SteerTowards((CohesionSum / CohesionCount) - Boid.Position, Boid) * CohesionFactor;
		if (SeparationCount > 0) // Don't get to close
			Boid.Acceleration += SteerTowards(SeparationSum / SeparationCount, Boid) * SeparationFactor;
		if (TargetPoint) // Move towards a point
			Boid.Acceleration += SteerTowards(MoveTowards(Boid), Boid) * MoveToTargetFactor;
		
		return;
	}


	//bool isAligning = FVector::DotProduct(Boid.Velocity - FVector(0, 0, Boid.Velocity.Z), AlignSum - FVector(0, 0, AlignSum.Z)) < .6;
	///*
	// Aligning adjusted with gravity in mind
	if (AlignCount > 0 && FVector::DotProduct((Boid.Velocity - FVector(0, 0, Boid.Velocity.Z)).GetSafeNormal(), (AlignSum -FVector(0, 0, AlignSum.Z).GetSafeNormal())) < .6) // Move in the same direction
		Boid.Acceleration += SteerTowards(AlignSum / AlignCount, Boid, true) * AlignmentFactor;

	// Flock together
	if (CohesionCount > 0 && true) 
	{
		Boid.Acceleration += SteerTowards((CohesionSum / CohesionCount) - Boid.Position, Boid, true) * CohesionFactor;
		CohesionCount = 0;
	}
	//*/

	//FVector SeparationWeightXY = ((FVector(1, 1, .1) * SeparationSum).GetSafeNormal() * SeparationSum.Size() / SeparationCount) - Boid.Position;
	FVector SeparationWeight = SteerTowards(SeparationSum / SeparationCount, Boid, true) * SeparationFactor;
	if (false) // Normal separation
	{
		if (SeparationCount > 0) // Don't collide
			Boid.Acceleration += SeparationWeight;
	}
	else // All birds will try to match z-value
	{
		FVector SeparationWeightXY = ((FVector(1, 1, zCohesion != 0 ? -zCohesion : 1) * SeparationSum).GetSafeNormal() * SeparationSum.Size() / SeparationCount); //.GetSafeNormal()*CohesionSum.Size()
		if (SeparationCount > 0) // Flock together
			Boid.Acceleration += SteerTowards(SeparationWeightXY, Boid, true) * SeparationFactor;
	}
	//if(TargetPoint)	Boid.Acceleration += SteerTowards(MoveTowards(Boid), Boid, true) * MoveToTargetFactor;
	//*/

	
	

	// Gravity
	FVector TargetPos = TargetPoint->GetActorLocation();
	//FVector TargetPos = Boid.savedLocation;
	//if (TargetPos == FVector::ZeroVector) TargetPos = TargetPoint->GetActorLocation();
	FVector TargetDirection = TargetPos - Boid.Position;

	if (HeightLoss != 0) //(Boid.Acceleration.Z < SpeedAvoidance && !Boid.Ascending) || (Boid.Acceleration.Z < -SpeedAvoidance && Boid.Ascending)
	{
		FVector dir;

		
		
		float zFactor = TargetDirection.Z;
		if (Boid.savedLocation != FVector::ZeroVector)
			zFactor = (Boid.savedLocation - Boid.Position).Z;

		// Could have done a random timer going upwards, checking afterwards if they need more lift or not
		if ((zFactor < SpeedAvoidance && !Boid.Ascending) || (zFactor < -SpeedAvoidance && Boid.Ascending))
		{
			Boid.Ascending = true;
			dir = SteerTowards(FVector(TargetDirection.GetSafeNormal2D().X, TargetDirection.GetSafeNormal2D().Y, 0)*1.4 + FVector(0, 0, .5), Boid, true) * HeightLoss * 3;

			Boid.Acceleration += FMath::Lerp(Boid.Velocity, dir, 0.2f);
		}
		else
		{
			Boid.Ascending = false;

			dir = SteerTowards(FVector(TargetDirection.GetSafeNormal2D().X, TargetDirection.GetSafeNormal2D().Y, -.15), Boid, true) * HeightLoss;

			//SteerTowards(FVector(0, 0, -9.81), Boid, true) * HeightLoss;

			Boid.Acceleration += FMath::Lerp(Boid.Velocity, dir, 0.2f);

		}

		// Invert Ascension bool if separation factor is working against you
		if (SeparationWeight.Length() > dir.Length() && FVector::DotProduct(SeparationWeight.GetSafeNormal(), dir.GetSafeNormal()) < 0)
		{
			Boid.Ascending = !Boid.Ascending;
			Boid.savedLocation = Boid.Position + FVector(0, 0, (dir - SeparationWeight).Z);
		}

		// If 
		if ((TargetPos-Boid.Position).Z < SpeedAvoidance && (TargetPos - Boid.Position).Z > -SpeedAvoidance)
		{
			Boid.savedLocation = FVector::ZeroVector;
		}

		// on top or below the flock
		if (CohesionSum.Z > CohesionSum.X + CohesionSum.Y)
		{
			Boid.MatchingTheFlock = true;
			//Boid.Acceleration += FRotator(0, 120, 0).RotateVector(FVector(Boid.Velocity.X, Boid.Velocity.Y, 0)) * 5; //RotateAngleAxis(90 /* CohesionSum.GetSafeNormal2D().X*/, FVector(0, 0, 0))
		}
		
		// Coheer on xy-plane
		FVector CohesionXY = ((FVector(1, 1, -5) * CohesionSum).GetSafeNormal() * CohesionSum.Size() / CohesionCount) - Boid.Position; //.GetSafeNormal()*CohesionSum.Size()
		if (CohesionCount > 0) // Flock together
			Boid.Acceleration += SteerTowards(CohesionXY, Boid, true) * CohesionFactor;

		//Boid.Acceleration += FMath::Lerp(Boid.Velocity, dir, 0.2f);

		//const TCHAR* BoolText = Boid.Ascending ? TEXT("True") : TEXT("False");
		//if (Boid.Ascending) UE_LOG(LogTemp, Warning, TEXT("Z: %.f, Ascending: %s"), Boid.Acceleration.Z, BoolText);
	}


	// Targetpoint adjusted with gravity calculations
	FVector TargetPointPos = TargetPoint->GetActorLocation();
	FVector u = (TargetPointPos - FVector(0, 0, TargetPointPos.Z)).GetSafeNormal();
	FVector v = Boid.Velocity - FVector(0, 0, Boid.Velocity.Z).GetSafeNormal();
	float xyDot = FVector::DotProduct(u, v);
	float fullDot = FVector::DotProduct(TargetPointPos.GetSafeNormal(), Boid.Velocity.GetSafeNormal());

	if (TargetPoint &&
		(((xyDot < .85 || fullDot < .8) && !Boid.TargetingTarget) || 
			((xyDot < .99 || fullDot < .95) && Boid.TargetingTarget))) // Move towards a point
	{
		Boid.TargetingTarget = true;
		Boid.Acceleration += SteerTowards(MoveTowards(Boid), Boid, true) * MoveToTargetFactor;
	}
	else
	{
		Boid.TargetingTarget = false;
	}
	//*/

	const TCHAR* BoolText = Boid.TargetingTarget ? TEXT("True") : TEXT("False");
	if (Boid.TargetingTarget)
		UE_LOG(LogTemp, Warning, TEXT("X: %.f, Y: %.f, Z: %.f. Targeting Target: %s"), Boid.Acceleration.X, Boid.Acceleration.Y, Boid.Acceleration.Z, BoolText);
}


FVector ACPP_FlockManager::SteerTowards(const FVector& DesiredDirection, const FBoid& Boid)
{
	FVector DesiredVelocity = DesiredDirection * Boid.MaxSpeed;
	FVector Steering = DesiredVelocity - Boid.Velocity;
	return Steering.GetClampedToMaxSize(Boid.MaxForce);
}

FVector ACPP_FlockManager::SteerTowards(const FVector& DesiredDirection, const FBoid& Boid, bool normal)
{
	FVector DesiredVelocity = DesiredDirection.GetSafeNormal() * Boid.MaxSpeed;
	FVector Steering = DesiredVelocity - Boid.Velocity;
	return Steering.GetClampedToMaxSize(Boid.MaxForce);
}


bool ACPP_FlockManager::IsHeadingForCollision(const FBoid& Boid) const
{
	FHitResult Hit;
	FVector Start = Boid.Position;
	FVector End = Start + Boid.Velocity.GetSafeNormal() * ObstacleAvoidanceThreshold;

	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	Params.AddIgnoredActor(this);

	return GetWorld()->SweepSingleByChannel(
		Hit,
		Start,
		End,
		FQuat::Identity,
		ObstacleChannel,
		FCollisionShape::MakeSphere(SphereCastRadius),
		Params
	);
}

//Starts checking for available directions to steer towards
FVector ACPP_FlockManager::ObstacleRays(const FBoid& Boid) const
{
	TArray<FVector> RayDirections = FCPP_BoidHelper::Directions;

	FRotator Rotation = Boid.Velocity.Rotation();
	
	for (int32 i = -1; i < RayDirections.Num(); i++)
	{
		FVector Dir = FVector::ZeroVector;
		if (i == -1)
		{
			Dir = FRotator(0, 0, 30).RotateVector(Boid.Velocity.GetSafeNormal());
		}
		else
			Dir = Rotation.RotateVector(RayDirections[i]);
		
		Dir.Normalize();


		
		FVector Start = Boid.Position;
		FVector End = Start + Dir * ObstacleAvoidanceThreshold;

		FHitResult Hit;
		FCollisionQueryParams Params;
		Params.bTraceComplex = false;
		Params.AddIgnoredActor(this);
		
		//Debug avoidance
		if (bShowDebugAvoidance)
		{
			DrawDebugLine(GetWorld(),Start,End,FColor::Red,false,-1.0f,0,2.0f);
		}
		
		if (!GetWorld()->SweepSingleByChannel(Hit,Start,End,FQuat::Identity,ObstacleChannel,FCollisionShape::MakeSphere(SphereCastRadius),Params))
		{
			return Dir;
		}
	}
	
	return Boid.Velocity;
}

FVector ACPP_FlockManager::MoveTowards(const FBoid& Boid) const
{
	FVector TargetPosition = TargetPoint->GetActorLocation();
	FVector Direction = TargetPosition - Boid.Position;

	if (Direction.Size() < 100)
	{
		return FVector::ZeroVector;
	}
	
	return Direction;
}

//JUST FOR TESTING
/*FVector ACPP_FlockManager::AvoidBoundary(FBoid& Boid) const
{

	FVector CenterPoint = GetActorLocation(); 
	FVector Direction = Boid.Position - CenterPoint;
	float Distance = Direction.Size();
    
	
	if (Distance > BoundaryRadius - BoundaryAvoidanceThreshold)
	{
		
		float Strength = FMath::Max(Distance - (BoundaryRadius - BoundaryAvoidanceThreshold), 0.f);
		FVector Steering = (-Direction.GetSafeNormal() * Boid.MaxSpeed) - Boid.Velocity;
		Steering = Steering.GetClampedToMaxSize(Boid.MaxForce);
		return Steering * MoveToTargetFactor * (Strength / BoundaryAvoidanceThreshold);
	}
    
	return FVector::ZeroVector;
}*/

