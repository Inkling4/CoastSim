// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AStarNode.generated.h"

// Enum for showing the node state for A*
UENUM(BlueprintType) 
enum class ENodeState : uint8
{
	Blocked,
	Open,
	Closed,
	Path,
	Start,
	End
};

class USphereComponent;
class UStaticMeshComponent;

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

	float FValue = -1;
	
	
	FTimerHandle NeighborDetectionTimerHandle;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<UStaticMeshComponent> MyRootComponent;

	// For neighbor node detection. Deletes itself after finding the neighbors.
	// Do not reference in code, as it deletes itself in BeginPlay().
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AStar")
	TObjectPtr<USphereComponent> SphereComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AStar")
	TArray<AAStarNode*> Neighbors;

	void FindNeighbors();

	virtual void BeginPlay() override;
	
	// State of this node
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AStar")
	ENodeState NodeState = ENodeState::Open;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AStar")
	bool bIsWalkable = true;
	
	float GValue = 0.f;
	
	// Decides how difficult it is to traverse this node, as a multiplier.
    	// Default value is 1.
    	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "AStar")
    	float TerrainDifficulty = 1.f;
	
public:
	AAStarNode();
	
	UPROPERTY(VisibleAnywhere, category = "AStar")
	int PathFindingDepth;
	
	// Returns the total cost from start point to this node.
	float GetGValue();
	
	void SetGValue(float InGValue);
	
	// Changes the color of the Node's mesh.
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AStar")
	void ChangeColor(FName InColor);
	

	// Returns the multiplier of terrain difficulty.
	float GetTerrainDifficulty();
	
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

	// Returns the F value of this node. 
	// F value is the sum of the heuristic cost, and the distance to this node from your start point.
	UFUNCTION()
	float GetFValue();
	
	// Sets the F value of this node.
	UFUNCTION()
	void SetFValue(float InFValue);
	

};
