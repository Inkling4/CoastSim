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
	PrimaryComponentTick.bCanEverTick = true;
	
	OwnerActor = GetOwner();

	
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
	if (TargetNode == nullptr) {return;}
	
	
}

void UAStarComponent::ChangeDirection()
{
	FVector2D CurrentLocation {OwnerActor->GetActorLocation().X, OwnerActor->GetActorLocation().Y};
	FVector2D NextGoalLocation {NextNode->GetActorLocation().X, NextNode->GetActorLocation().Y};
	
	FVector2D NewDirection;
	NewDirection.X = NextGoalLocation.X - CurrentLocation.X;
	NewDirection.Y = NextGoalLocation.Y - CurrentLocation.Y;
	
	// Turns it into unit vector
	NewDirection /= NewDirection.Length();
	
	MovementDirection = NewDirection;
}

// Called every frame
void UAStarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (OwnerActor != nullptr && NextNode != nullptr)
	{
		if (bIsMoving)
		{
			// Called to change the direction vector
			ChangeDirection();
			
			
			// Gets distance to next node
			float oldDistance = NextNode->GetHeuristicCost(FVector2D {OwnerActor->GetActorLocation().X, OwnerActor->GetActorLocation().Y});
			
			
			FVector CurrentLocation {OwnerActor->GetActorLocation().X, OwnerActor->GetActorLocation().Y, OwnerActor->GetActorLocation().Z};
			CurrentLocation.X += MovementDirection.X * MovementSpeed * DeltaTime;
			CurrentLocation.Y += MovementDirection.Y * MovementSpeed * DeltaTime;
			
			// Sets new location
			OwnerActor->SetActorLocation(CurrentLocation);
			
			float newDistance = NextNode->GetHeuristicCost(FVector2D {OwnerActor->GetActorLocation().X, OwnerActor->GetActorLocation().Y});
			
			// Checks if you have passed over the node
			if (newDistance > oldDistance || newDistance <= 10.f)
			{
				if (NextNode != GoalNode)
				{
					NextNode = MovementQueue.top();
					MovementQueue.pop();
				}
				
			}
			
			
		}
        	
	}
	
	

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
	
	TObjectPtr<AAStarNode> StartNode = GetCurrentNode();
	if (StartNode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could not find starter node to pathfind from!"));
		return;
	}
	
	
	
	
	
	
	
	
	
}

