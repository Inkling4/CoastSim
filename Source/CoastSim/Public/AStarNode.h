// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AStarNode.generated.h"


// Enum for showing the node state for A*
UENUM(BlueprintType) 
enum class ENodeState : uint8
{
	Default,
	Blocked,
	Open,
	Closed,
	Path,
	Start,
	End
};

UCLASS(BlueprintType)
class COASTSIM_API AAStarNode : public AActor
{
	GENERATED_BODY()
protected:
	

	
public:
	AAStarNode();
	
	// Returns the Heuristic of this node in cm for the given location to travel.
	// Disregards the Z axis.
	// InGoalLocation: Coordinates you wish to go to.
	UFUNCTION(BlueprintCallable, category = "AStar")
	float GetHeuristicCost(FVector2D InGoalLocation);
	
	
	
};
