// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarNode.h"
#include "Kismet/KismetMathLibrary.h" // For square root function
#include "Components/SphereComponent.h"

AAStarNode::AAStarNode()
{
	PrimaryActorTick.bCanEverTick = false;

	// Creates sphere for neighbor detection.
	SphereComponent = CreateDefaultSubobject<USphereComponent>("SphereComponent");
	SphereComponent->SetupAttachment(RootComponent);
}

void AAStarNode::BeginPlay()
{
	Super::BeginPlay();
	// Calls FindNeighbors after a small time to make the detection actually find everyone properly.
	// THIS DELETES THE SPHERE! DON'T USE THE POINTER FOR OTHER THINGS!!
	GetWorld()->GetTimerManager().SetTimer(NeighborDetectionTimerHandle, this, &AAStarNode::FindNeighbors, 0.1f, false);
	
}

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
	FVector2D nodeLocation {GetActorLocation().X, GetActorLocation().Y};
	FVector2D distanceVector {0, 0};

	// Finds the vector between the goal point and this node.
	// Negative numbers won't matter, as they will become positive through pythagoras math
	distanceVector.X = nodeLocation.X - InGoalLocation.X;
	distanceVector.Y = nodeLocation.Y - InGoalLocation.Y;

	// Calculates the distance using pythagoras.
	float heuristic = UKismetMathLibrary::Sqrt(distanceVector.X * distanceVector.X + distanceVector.Y * distanceVector.Y);
	
	// UE_LOG(LogTemp, Display, TEXT("Node %s has calculated heuristic as %f."), *FString(GetName()), heuristic);
	
	return heuristic;

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
}
void AAStarNode::EnableNode()
{
	bIsWalkable = true;
	NodeState = ENodeState::Open;
}


