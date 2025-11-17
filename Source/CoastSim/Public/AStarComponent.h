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
	// Called when the game starts
	virtual void BeginPlay() override;
	

	UPROPERTY()
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
	void PathFindTo(const AAStarNode* AStarNode);
	
	

		
};
