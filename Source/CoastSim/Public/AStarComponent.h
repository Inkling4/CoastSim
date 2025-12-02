// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include <queue>
#include "AStarComponent.generated.h"

/*
 * This component will be used to store variables and functions for finding a path using A*.
 * 
 *
 */

class AAStarNode;
class AAStarGlobals;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COASTSIM_API UAStarComponent : public UActorComponent
{
	GENERATED_BODY()


public:	
	// Sets default values for this component's properties
	UAStarComponent();
	
protected:
	
	// For movement purposes
	UPROPERTY()
	TObjectPtr<AAStarNode> PreviousNode;
	
	// Detection radius, decides if it has reached the goal
	UPROPERTY(EditAnywhere, category = "AStar")
	float NodeDetectionRadius = 30.f;
	
	// Nodes to explore. Array used for pathfinding.
	UPROPERTY()
	TArray<AAStarNode*> NodesToExplore;
	
	// Array of successful finds in the pathfinding.
	UPROPERTY(VisibleAnywhere, category = "AStar")
	TArray<AAStarNode*> NodesExplored;
	
	// Given a list of astarnodes, returns the one with the smallest f value.
	UFUNCTION()
	AAStarNode* GetBestNode(TArray<AAStarNode*> InAStarNodes);
	
	
	// Changes direction, so it points to the next node for movement.
	void ChangeDirection();
	
	// The actor that has this component attached.
	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Movement speed for A* movement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "AStar")
	float MovementSpeed = 500.f;
	// If true, will move in movementdirection. Only set as true when it has a goal to move towards.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AStar")
	bool bIsMoving = false;
	// Current direction of movement as a vector. Should be between -1 and 1
	UPROPERTY(VisibleAnywhere, category = "AStar")
	FVector2D MovementDirection {0, 0};
	// The next node to move to
	UPROPERTY()
	AAStarNode* NextNode;
	// The end goal node.
	UPROPERTY()
	AAStarNode* GoalNode;

	// Pointer to the AStarGlobals class, which stores an array of all nodes.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "AStar")
	TObjectPtr<AAStarGlobals> AStarGlobals;
	
	// Returns the closest node by XY location to the owner actor.
	UFUNCTION()
	AAStarNode* GetCurrentNode();
	
	// Queue for movement. Uses standard C++ library.
	// Would have used a priority queue, but this works fine.
	std::queue<AAStarNode*> MovementQueue;
	
	
	

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Call this function to move the actor to the node specified, using pathfinding.
	UFUNCTION(BlueprintCallable, category = "AStar")
	void PathFindTo(AAStarNode* AStarNode);
	
		
	// Deprecated:
	
	/*
	 
	 // Moves owner actor to AStarNode
      	UFUNCTION()
      	void AStarMoveTo (const AAStarNode* TargetNode);
      	
	UPROPERTY()
	AAStarNode* CurrentNode;
	
	
	*/
		
};
