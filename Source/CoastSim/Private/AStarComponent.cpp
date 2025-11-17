// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarComponent.h"
#include "AStarNode.h"
#include "Kismet/GameplayStatics.h"
#include "AStarGlobals.h"

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
	
	if (TObjectPtr<AActor> GlobalsActor = UGameplayStatics::GetActorOfClass(GetWorld()->GetCurrentLevel(), AAStarGlobals::StaticClass()))
	{
		AStarGlobals = Cast<AAStarGlobals>(GlobalsActor);
	}

	
}

void UAStarComponent::AStarMoveTo(const AAStarNode* TargetNode)
{
	TObjectPtr<AActor> OwnerActor = GetOwner();
	
	
}


// Called every frame
void UAStarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}

// Finds the closest node to the owner of this component
AAStarNode* UAStarComponent::GetCurrentNode()
{
	FVector2D ActorLocation  {GetOwner()->GetActorLocation().X, GetOwner()->GetActorLocation().Y};
	if (AStarGlobals == nullptr) {return nullptr;}
	
	
	float shortestDistance = 65535.f; // High value for comparison purposes
	AAStarNode* closestNode = nullptr;
	TArray<AAStarNode*> AStarNodes = AStarGlobals->GetAStarNodes();
	
	// Increments through all AStarNodes and finds the closest one to this component's owner
	for (AAStarNode* AStarNode : AStarNodes)
	{
		float DistanceToNode = AStarNode->GetHeuristicCost(ActorLocation);
		if (DistanceToNode < shortestDistance)
		{
			closestNode = AStarNode;
			shortestDistance = DistanceToNode;
			
			if (DistanceToNode <= 10.f) // Stops early if it finds a "really" close one
			{
				return closestNode;
			}
		} 
	}
	
	if (closestNode == nullptr){ return nullptr; }
	
	return closestNode;
	
}


void UAStarComponent::PathFindTo(const AAStarNode* AStarNode)
{
	if (AStarNode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AStarNode goal in pathfinding is null."));
		return;
	}
	
	TObjectPtr<AActor> OwnerActor = GetOwner();
	
	TObjectPtr<AAStarNode> StartNode = GetCurrentNode();
	if (StartNode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find starter node to pathfind from!"));
		return;
	}
	
	
	
	
	
	
	
	
	
}

