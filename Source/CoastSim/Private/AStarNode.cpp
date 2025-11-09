// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarNode.h"

#include "DynamicMesh/DynamicMesh3.h"
#include "Kismet/KismetMathLibrary.h"

AAStarNode::AAStarNode()
{
	PrimaryActorTick.bCanEverTick = false;

	
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

	
	UE_LOG(LogTemp, Display, TEXT("Node %s has calculated heuristic as %f."), *FString(GetName()), heuristic);
	
	return heuristic;

}
