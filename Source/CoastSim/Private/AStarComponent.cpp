// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarComponent.h"
#include "AStarNode.h"

// Sets default values for this component's properties
UAStarComponent::UAStarComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	
}


// Called when the game starts
void UAStarComponent::BeginPlay()
{
	Super::BeginPlay();

	
}


// Called every frame
void UAStarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UAStarComponent::PathFindTo(TObjectPtr<AStarNode> AStarNode)
{
	if (AStarNode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AStarNode goal in pathfinding is null."));
		return;
	}
	
}

