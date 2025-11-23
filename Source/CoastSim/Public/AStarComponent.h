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
	
	UPROPERTY(VisibleAnywhere, category = "AStar")
	TArray<AAStarNode*> NodesToExplore;
	
	UPROPERTY(VisibleAnywhere, category = "AStar")
	TArray<AAStarNode*> NodesExplored;
	
	// Given a list of astarnodes, returns the one with the smallest f value.
	UFUNCTION()
	AAStarNode* GetBestNode(TArray<AAStarNode*> InAStarNodes);
	
	
	// Changes direction for next node
	void ChangeDirection();
	
	// The actor that has this component attached.
	UPROPERTY()
	TObjectPtr<AActor> OwnerActor;
	
	// Called when the game starts
	virtual void BeginPlay() override;
	
	// Movement speed for A* movement
	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "AStar")
	float MovementSpeed = 100.f;
	// If true, will move in movementdirection
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, category = "AStar")
	bool bIsMoving = false;
	// Current direction of movement. Should be between -1 and 1
	UPROPERTY(VisibleAnywhere, category = "AStar")
	FVector2D MovementDirection {0, 0};
	// The next node to move to
	UPROPERTY()
	AAStarNode* NextNode;
	// The end goal
	UPROPERTY()
	AAStarNode* GoalNode;
	
	UPROPERTY()
	AAStarNode* CurrentNode;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "AStar")
	TObjectPtr<AAStarGlobals> AStarGlobals;
	
	UFUNCTION()
	AAStarNode* GetCurrentNode();
	
	// Priority queue for movement. Uses standard C++ library
	std::priority_queue<AAStarNode*> MovementQueue;
	
	// Moves owner actor to AStarNode
	UFUNCTION()
	void AStarMoveTo (const AAStarNode* TargetNode);

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Call this function to move the actor to the node specified, using pathfinding.
	UFUNCTION(BlueprintCallable, category = "AStar")
	void PathFindTo(AAStarNode* AStarNode);
	
	

		
};
