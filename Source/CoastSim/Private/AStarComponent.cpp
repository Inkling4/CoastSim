// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarComponent.h"
#include "AStarNode.h"
#include "Kismet/GameplayStatics.h"
//#include "Kismet/KismetMathLibrary.h"
#include "UObject/UObjectGlobals.h"
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
	// Being double sure this runs
	OwnerActor = GetOwner();
	
	if (TObjectPtr<AActor> GlobalsActor = UGameplayStatics::GetActorOfClass(GetWorld()->GetCurrentLevel(), AAStarGlobals::StaticClass()))
	{
		AStarGlobals = Cast<AAStarGlobals>(GlobalsActor);
	}
	
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No AStar Component found!"));
	}

	
}

void UAStarComponent::AStarMoveTo(const AAStarNode* TargetNode)
{
	if (TargetNode == nullptr) {return;}
	
	
}

// Currently broken
void UAStarComponent::ChangeDirection()
{
	// Z values as 0 to make the kismet math function work
	FVector CurrentLocation {OwnerActor->GetActorLocation().X, OwnerActor->GetActorLocation().Y, 0.f};
	FVector NextGoalLocation {NextNode->GetActorLocation().X, NextNode->GetActorLocation().Y, 0.f};


	FVector TempNewDirection = (NextGoalLocation - CurrentLocation).GetSafeNormal();
	
		//UKismetMathLibrary::GetDirectionUnitVector(CurrentLocation, NextGoalLocation);
			// Applies changes to the movement direction property
	FVector2D NewDirection {TempNewDirection.X, TempNewDirection.Y};
	// GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green, FString::Printf(TEXT("Goal location Y coords are: %f %f"), NextGoalLocation.X, NextGoalLocation.Y));
	MovementDirection = NewDirection;
}	

// Called every frame
void UAStarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (bIsMoving)
	{
		if (OwnerActor != nullptr && NextNode != nullptr)
		{
			
			// Called to change the direction vector
			ChangeDirection();
			
			
			// Gets distance to next node
			float oldDistance = NextNode->GetHeuristicCost(FVector2D {OwnerActor->GetActorLocation().X, OwnerActor->GetActorLocation().Y});
			
			
			FVector CurrentLocation {OwnerActor->GetActorLocation()};
			CurrentLocation.X += MovementDirection.X * MovementSpeed * DeltaTime;
			CurrentLocation.Y += MovementDirection.Y * MovementSpeed * DeltaTime;
			
			// Sets new location
			OwnerActor->SetActorLocation(CurrentLocation);
			//GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Blue, TEXT("Movement called!"));
			
			float newDistance = NextNode->GetHeuristicCost(FVector2D {OwnerActor->GetActorLocation().X, OwnerActor->GetActorLocation().Y});
			
			// Checks if you have passed over the node
			if (newDistance > oldDistance || newDistance <= NodeDetectionRadius)
			{
				if (!MovementQueue.empty())
				{
					NextNode = MovementQueue.front();
					MovementQueue.pop();
				}
				else
				{
					bIsMoving = false;
				}
			}
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Either OwnerActor or NextActor is null!"));
		}
	}
	
	
	

}

// Finds the closest node to the owner of this component
AAStarNode* UAStarComponent::GetCurrentNode()
{
	FVector2D ActorLocation  {OwnerActor->GetActorLocation().X, OwnerActor->GetActorLocation().Y};
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
			
			if (DistanceToNode <= NodeDetectionRadius) // Stops early if it finds a "really" close one
			{
				return closestNode;
			}
		} 
	}
	
	if (closestNode == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, TEXT("Finding closest node failed!"));
		return nullptr;
	}
	
	return closestNode;
	
}

AAStarNode* UAStarComponent::GetBestNode(TArray<AAStarNode*> InAStarNodes)
{
	AAStarNode* bestNode = nullptr;
	for (auto Node : InAStarNodes)
	{
		if (bestNode == nullptr)
		{
			bestNode = Node;
		}
		else
		{
			if (bestNode->GetFValue() > Node->GetFValue()) // Compares F Values.
			{
				bestNode = Node;
			}
		}
	}
	
	return bestNode;
}


void UAStarComponent::PathFindTo(AAStarNode* AStarNode)
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
	
	
	bIsMoving = false;

	for (auto Node : NodesExplored)
	{
		Node->ChangeColor("White");
	}
	
	
	
	NodesToExplore.Empty(); // Empties the nodes to explore list.
	NodesExplored.Empty(); // Empties the "explored" list.
	GoalNode = AStarNode;
	
	// The pathfinding starts here.
	StartNode->SetFValue(0); // Sets f value to 0 as it's the starter node.
	StartNode->SetGValue(0);
	StartNode->PathFindingDepth = 0;
	NodesToExplore.AddUnique(StartNode);
	
	FVector2D GoalNodeLocation {GoalNode->GetActorLocation().X, GoalNode->GetActorLocation().Y};
	
	
	
	while (!NodesToExplore.IsEmpty())
	{
		TObjectPtr<AAStarNode> ThisNode = GetBestNode(NodesToExplore);
		
		for (auto Neighbor : ThisNode->GetNeighbors())
		{
			if (!NodesToExplore.Contains(Neighbor) && !NodesExplored.Contains(Neighbor))
			{
				
				FVector2D NeighborLocation {Neighbor->GetActorLocation().X, Neighbor->GetActorLocation().Y};
				
				if (Neighbor->GetIsWalkable())
				{
					if (Neighbor == GoalNode)
					{
						Neighbor->PathFindingDepth = ThisNode->PathFindingDepth + 1;
						NodesExplored.AddUnique(ThisNode);
						NodesExplored.AddUnique(Neighbor);
						NodesToExplore.Empty();
						break;
					}
					else
						// Sets F Values on the neighbor nodes.
					{
						// The G Value of the current node (total cost from start node)
						float PreviousGValue = CurrentNode->GetGValue();
						// Multiplier for terrain difficulty.
						float TerrainDifficulty = CurrentNode->GetTerrainDifficulty();
                    				
						// Distance to the neighbor from the current node
						float DistanceToNeighbor = CurrentNode->GetHeuristicCost(NeighborLocation);
                    					
						float NeighborGValue = PreviousGValue + (DistanceToNeighbor * TerrainDifficulty);
                    					
						Neighbor->SetGValue(NeighborGValue);
						Neighbor->SetFValue(Neighbor->GetHeuristicCost(GoalNodeLocation) + NeighborGValue);
						Neighbor->PathFindingDepth = CurrentNode->PathFindingDepth + 1;
                    					
						// Adds node to the exploration list.
						NodesToExplore.AddUnique(Neighbor);
					}
				}
				
				
			}
			
		}
		
	
		if (!NodesToExplore.IsEmpty())
		{
			NodesToExplore.RemoveSingle(CurrentNode);
			NodesExplored.AddUnique(CurrentNode);
		}
	}

	// Empties queue
	MovementQueue = std::queue<AAStarNode*>();
	
	{
		for (int i = 0; i < GoalNode->PathFindingDepth; i++)
		{
			TObjectPtr<AAStarNode> bestNodeOfDepth;
			TArray<AAStarNode*> NodesOfDepth;
			
			for (auto Node : NodesExplored)
			{
				if (Node->PathFindingDepth == i)
				{
					NodesOfDepth.AddUnique(Node);
				}
			}

			bestNodeOfDepth = GetBestNode(NodesOfDepth);
			MovementQueue.push(bestNodeOfDepth);
			bestNodeOfDepth->ChangeColor("Red");
		}
		
		/*
		for (auto Node : NodesExplored)
        	{
        		MovementQueue.push(Node);
        		Node->ChangeColor("Red");
        	}
		*/
	}
	
	
	// Starts movement
	NextNode = StartNode;
	PreviousNode = nullptr;
	bIsMoving = true;
	
}

