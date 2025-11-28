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

class USphereComponent;

/*
 * Nodes for A* pathfinding.
 * Please make sure to place them in a grid, so the spehere component detects nodes properly.
 *
 */

UCLASS(BlueprintType)
class COASTSIM_API AAStarNode : public AActor
{
	GENERATED_BODY()
protected:

	FTimerHandle NeighborDetectionTimerHandle;

	// For neighbor node detection
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AStar")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AStar")
	TArray<AAStarNode*> Neighbors;

	void FindNeighbors();

	virtual void BeginPlay() override;
	
	// State of this node
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AStar")
	ENodeState NodeState = ENodeState::Default;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AStar")
	bool bIsWalkable = true;
	
	
	
public:
	AAStarNode();

	// Decides how difficult it is to traverse this node, as a multiplier.
	// Default value is 1.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "AStar")
	float TerrainDifficulty = 1.f;
	
	// Returns the current state of this node.
	ENodeState GetNodeState() const;

	bool GetIsWalkable();
	
	// Returns the neighbor nodes of this node in eight directions.
	TArray<AAStarNode*> GetNeighbors();
	
	// Disables node, makes it unwalkable.
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "AStar")
	void DisableNode();
	// Makes the node walkable.
	UFUNCTION(CallInEditor, BlueprintCallable, Category = "AStar")
	void EnableNode();
	
	// Returns the Heuristic of this node in cm for the given location to travel.
	// Disregards the Z axis, only use X and Y.
	// InGoalLocation: Coordinates you wish to go to.
	UFUNCTION(BlueprintCallable, category = "AStar")
	float GetHeuristicCost(FVector2D InGoalLocation);

	

};
