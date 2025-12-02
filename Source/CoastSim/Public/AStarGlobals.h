// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AStarGlobals.generated.h"

class AAStarNode;

UCLASS()
class COASTSIM_API AAStarGlobals : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAStarGlobals();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// An array of all AStarNodes  in the scene. 
	// Using this class to store them so they won't be stored in every single AStarComponent if there are multiple.
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, category = "AStar")
	TArray<AAStarNode*> AStarNodes;
	
	TArray<AAStarNode*> GetAStarNodes();

};
