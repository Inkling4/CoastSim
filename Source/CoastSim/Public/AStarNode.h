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
	
public:	
	AAStarNode(const FObjectInitializer& ObjectInitializer);

	virtual void OnConstruction(const FTransform& Transform) override;
	
	void CalculateFCost();

	void SetState(ENodeState NewState);

	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	void ApplyColor(const FLinearColor& Color);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
	int32 X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
	int32 Y = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
	bool bIsWalkable = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
	float TileCost = 1.0f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Node")
	float GCost = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Node")
	float HCost = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Node")
	float FCost = 0.f;

	UPROPERTY()
	AAStarNode* Parent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* TileMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="A* Node")
	ENodeState CurrentState = ENodeState::Default;
	
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;
};
