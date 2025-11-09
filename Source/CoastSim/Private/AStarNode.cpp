// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarNode.h"


AAStarNode::AAStarNode(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	TileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("TileMesh"));
	SetRootComponent(TileMesh);

	TileMesh->SetMobility(EComponentMobility::Static);
	TileMesh->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	TileMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	TileMesh->SetCollisionResponseToChannel(ECC_Visibility, ECollisionResponse::ECR_Block);
	TileMesh->bHiddenInGame = false;

	
}

void AAStarNode::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (!DynamicMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(TileMesh->GetMaterial(0), this);
		TileMesh->SetMaterial(0, DynamicMaterial);
	}

	SetState(bIsWalkable ? ENodeState::Default : ENodeState::Blocked);
	CalculateFCost();
}

void AAStarNode::CalculateFCost()
{
	FCost = GCost + HCost;

}

void AAStarNode::ApplyColor(const FLinearColor& Color)
{
	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(TEXT("Color"), Color);
	}
}

void AAStarNode::SetState(ENodeState NewState)
{
	CurrentState = NewState;

	switch (NewState)
	{
	case ENodeState::Default:
		ApplyColor(FLinearColor::White);
		break;
	case ENodeState::Blocked:
		ApplyColor(FLinearColor::Red);
		break;
	case ENodeState::Open:
		ApplyColor(FLinearColor::Yellow);
		break;
	case ENodeState::Closed:
		ApplyColor(FLinearColor::Blue);
		break;
	case ENodeState::Path:
		ApplyColor(FLinearColor::Green);
		break;
	case ENodeState::Start:
		ApplyColor(FLinearColor(0, 255, 255, 255));
		break;
	case ENodeState::End:
		ApplyColor(FLinearColor(195, 115, 0, 255));
		break;
	default:
		break;
	}
}

void AAStarNode::NotifyActorOnClicked(FKey ButtonPressed)
{
	Super::NotifyActorOnClicked(ButtonPressed);

	bIsWalkable = !bIsWalkable;
	SetState(bIsWalkable ? ENodeState::Default : ENodeState::Blocked);
}
