// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarNode.h"
#include "Kismet/KismetMathLibrary.h" // For square root function
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AAStarNode::AAStarNode()
{
	PrimaryActorTick.bCanEverTick = false;

	
	MyRootComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MyRootComponent"));
	SetRootComponent(MyRootComponent);
	
	// Creates sphere for neighbor detection.
	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetupAttachment(RootComponent);
	
}

float AAStarNode::GetTerrainDifficulty()
{
	return TerrainDifficulty;
}

float AAStarNode::GetGValue()
{
	return GValue;
}

void AAStarNode::SetGValue(float InGValue)
{
	GValue = InGValue;
}

void AAStarNode::BeginPlay()
{
	Super::BeginPlay();
	// Calls FindNeighbors after a small time to make the detection actually find everyone properly.
	// THIS DELETES THE SPHERE! DON'T USE THE POINTER FOR OTHER THINGS!!
	GetWorld()->GetTimerManager().SetTimer(NeighborDetectionTimerHandle, this, &AAStarNode::FindNeighbors, 0.1f, false);
	
}

// Function that adds every A* node it overlaps with to an array. This should be up to 8 nodes surrounding it,
// if the level designer is competent.
void AAStarNode::FindNeighbors()
{
	if (SphereComponent == nullptr){ return;}
	
	TArray<AAStarNode*> neighbors;
        
    	TArray<AActor*> OverlappingActors;
    	SphereComponent->GetOverlappingActors(OverlappingActors, AAStarNode::StaticClass());
        	
    	for (AActor* Node : OverlappingActors)
    	{
    		AAStarNode* neighbor = Cast<AAStarNode>(Node);
    		if (neighbor != this)
    		{
    			neighbors.Add(neighbor);
    		}
    		
    	}
    	Neighbors = neighbors;
		
		
		SphereComponent->DestroyComponent();
}




float AAStarNode::GetHeuristicCost(FVector2D InGoalLocation)
{

	
	// Current location
	const FVector2D nodeLocation {GetActorLocation().X, GetActorLocation().Y};
	// Target location
	const FVector2D goalLocation = InGoalLocation;
	
	FVector2D distanceVector;
	

	// Finds the vector between the goal point and this node.
	// Negative numbers won't matter, as they will become positive through pythagoras math
	distanceVector.X = nodeLocation.X - goalLocation.X;
	distanceVector.Y = nodeLocation.Y - goalLocation.Y;

	// Calculates the distance using pythagoras.
	float heuristic = UKismetMathLibrary::Sqrt(distanceVector.X * distanceVector.X + distanceVector.Y * distanceVector.Y);
	
	// UE_LOG(LogTemp, Display, TEXT("Node %s has calculated heuristic as %f."), *FString(GetName()), heuristic);
	
	return heuristic;

}

float AAStarNode::GetFValue()
{
	if (FValue == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Retreived F Value of -1!"));
	}
	return FValue;
}

void AAStarNode::SetFValue(float InFValue)
{
	FValue = InFValue;
}

TArray<AAStarNode*> AAStarNode::GetNeighbors()
{
	return Neighbors;
}




ENodeState AAStarNode::GetNodeState() const
{
	return NodeState;
}

bool AAStarNode::GetIsWalkable()
{
	return bIsWalkable;
}

void AAStarNode::DisableNode()
{
	bIsWalkable = false;
	NodeState = ENodeState::Blocked;
	ChangeColor("Disabled");
}
void AAStarNode::EnableNode()
{
	bIsWalkable = true;
	NodeState = ENodeState::Open;
	ChangeColor("White");
}


