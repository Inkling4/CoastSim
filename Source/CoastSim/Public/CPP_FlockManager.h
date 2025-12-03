// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FSpatialHashGrid.h"
#include "Engine/TargetPoint.h"
#include "CPP_FlockManager.generated.h"

class UInstancedStaticMeshComponent;
class CPP_BoidHelper;

USTRUCT(BlueprintType)

struct FBoid
{
	GENERATED_BODY()

#pragma region Boid Info Variables

	//Stores the previous and target position/rotation, used in tick to interpolate movement between each UpdateBoid()
	UPROPERTY(VisibleAnywhere, Category="Flock Settings")
	FVector PrevPosition;
	 
	UPROPERTY(VisibleAnywhere, Category="Flock Settings")
	FVector TargetPosition;

	UPROPERTY(VisibleAnywhere, Category="Flock Settings")
	FQuat PrevRotation;
	 
	UPROPERTY(VisibleAnywhere, Category="Flock Settings")
	FQuat TargetRotation;


	//The current position of the boid in world space
	UPROPERTY(VisibleAnywhere, Category="Flock Settings")
	FVector Position;

	//The current velocity of the boid
	UPROPERTY(VisibleAnywhere, Category="Flock Settings")
	FVector Velocity;

	//The current acceleration of the boid
	UPROPERTY(VisibleAnywhere, Category="Flock Settings")
	FVector Acceleration;


	//Used to check if a boid has changed cell
	UPROPERTY()
	FIntVector OldCellLocation;

	bool Ascending, TargetingTarget, MatchingTheFlock;

	FVector savedLocation;

#pragma endregion



	//The min/max speed/force to clamp velocity
	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float MaxSpeed;

	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float MinSpeed;

	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float MaxForce;

	
	FBoid()
		: PrevPosition(FVector::ZeroVector)
		, TargetPosition(FVector::ZeroVector)
		, PrevRotation(FQuat::Identity)
		, TargetRotation(FQuat::Identity)
		, Position(FVector::ZeroVector)
		, Velocity(FVector::ZeroVector)
		, Acceleration(FVector::ZeroVector)
		, MaxSpeed(500.f)
		, MinSpeed(300.0f)
		, MaxForce(300.f)
		, OldCellLocation(FVector::ZeroVector)
		, Ascending(false)
		, savedLocation(FVector::ZeroVector)
	{}
};

UCLASS()
class COASTSIM_API ACPP_FlockManager : public AActor
{
	GENERATED_BODY()

	FTimerHandle BoidUpdateTimerHandle;
	
public:
	// Sets default values for this actor's properties
	ACPP_FlockManager();
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//The struct that holds the data for each boid
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Flock Settings")
	FBoid BoidStruct;

	//Struct for the spatial hash grid, holds variables and methods used to generate, update and delete grid
	FSpatialHashGrid SpatialHashGrid;
	
	//ActorScene Components Assignable in blueprint
	UPROPERTY(VisibleAnywhere)
	UInstancedStaticMeshComponent* InstancedMesh;

	// A point in the world which the flock move towards
	UPROPERTY(EditAnywhere)
	AActor* TargetPoint;

	//The mesh thats assigned to the instancedMesh
	UPROPERTY(EditAnywhere, Category="Flock Settings")
	UStaticMesh* BoidMesh;

	//The array of boids in the flock
	TArray<FBoid> Boids;

	//A buffer array of the boids used in paralell for multithreading
	TArray<FBoid> BoidsBuffer;
	
	//Bool to show debugs for object avoidance and spatial hash grid
	UPROPERTY(EditAnywhere, Category="Flock Settings")
	bool bShowDebugAvoidance;

	UPROPERTY(EditAnywhere, Category="Flock Settings")
	bool bDrawGrid;


#pragma region Boid_Tuning_Variables //Variables used to change the behaviour and strength of the boid rules 

	//Set the number of boids in the flock
	UPROPERTY(EditAnywhere, Category="Flock Settings")
	int32 NumberOfBoids = 50;
	
	//Threshold sets the distance in which each rule gets "activated" to affect the flocking behaviour
	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float AlignThreshold = 600.f;

	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float CohesionThreshold = 800.f;

	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float SeparationThreshold = 300.f;

	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float ObstacleAvoidanceThreshold = 500.f;

	
	//Factor sets the strength of each rule
	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float AlignmentFactor;

	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float CohesionFactor;

	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float SeparationFactor;

	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float MoveToTargetFactor;

	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float ObstacleAvoidanceFactor;


	//How often the boid algorithm should run, standard is 0.033 which equals to roughly 30 fps
	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float BoidUpdateInterval;
	
	//Counter used in tick to fire UpdateBoid()
	float BoidUpdateAccumulator;
	
	//Radius for the sphere cast used for obstacle avoidance, should be about the size of the bird
	UPROPERTY(EditAnywhere, Category="Flock Settings")
	float SphereCastRadius = 30.f;

	//CollisionChannel for the obstacle avoidance, should be WorldStatic 
	UPROPERTY(VisibleAnywhere, Category="Flock Settings")
	TEnumAsByte<ECollisionChannel> ObstacleChannel = ECC_WorldStatic;


	//NOT IN USE
	//Used by Avoid Boundary to set the radius in which to contain the boids
	//UPROPERTY(EditAnywhere, Category="Flock Settings") float BoundaryRadius = 2000.f;
	//UPROPERTY(EditAnywhere, Category="Flock Settings")float BoundaryAvoidanceThreshold = 500.f;


	// Factor in which the bird loses height
	UPROPERTY(EditAnywhere, Category = "Flock Settings")
	float HeightLoss = 10.f;
	UPROPERTY(EditAnywhere, Category = "Flock Settings")
	float SpeedAvoidance = -70;

	UPROPERTY(EditAnywhere, Category = "Flock Settings") float zCohesion = 3;

	UPROPERTY(EditAnywhere, Category = "Flock Settings") bool oldFlocking;

	
#pragma endregion Boid_Tuning_Variables

	//Spawns and registers boids in the system, called in Begin Play
	void InitializeBoids();

	//The method used to run the boid algorithm, update and apply all forces to all boids. Gets called on a timer inside tick
	void UpdateBoids(float DeltaTime);

	//Method used to calculate and apply all forces on the boids algorithm, called inside UpdateBoids()
	void ApplyFlockingForces(FBoid& Boid, int32 BoidIndex, const TArray<int32>& NeighbourIndecies, TArrayView<const FBoid> BoidsView ) const;

	//Method used to check for obstacles, called inside UpdateBoids()
	bool IsHeadingForCollision(const FBoid& Boid) const;

	//Method used by ApplyFlockingForces to apply the steering vectors to actual acceleration on the boids
	static FVector SteerTowards(const FVector& DesiredDirection, const FBoid& Boid);

	static FVector SteerTowards(const FVector& DesiredDirection, const FBoid& Boid, bool normal);

	//Method called when IsHeadingForCollision is true, uses CPP_BoidHelper to cast out sphere casts along points on a sphere.
	//Starts centrally then gradually expands. Returns the first avaliable direction 
	FVector ObstacleRays(const FBoid& Boid) const;

	//Method used to return the direction to the given target point, used to direct the entire bird flock
	FVector MoveTowards(const FBoid& Boid) const;

	//FVector AvoidBoundary(FBoid& Boid) const;

};
