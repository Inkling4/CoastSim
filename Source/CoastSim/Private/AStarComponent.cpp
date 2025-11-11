// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarComponent.h"
#include "AStarNode.h"
#include "Kismet/GameplayStatics.h"

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

	
	// Gets all AStarNodes, and adds them to array.
	TArray<AActor*> AStarActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld()->GetCurrentLevel(), AActor::StaticClass(), AStarActors);
	for (auto Node : AStarActors)
	{
		AAStarNode* CastedNode;
		// Does not add actor to the array if it is of the wrong class.
		CastedNode = Cast<AAStarNode>(Node);
		if (CastedNode != nullptr)
		{
			AStarNodes.Add(CastedNode);
		}
	}

	
	
}




// Called every frame
void UAStarComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UAStarComponent::PathFindTo(TObjectPtr<AAStarNode> AStarNode)
{
	if (AStarNode == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("AStarNode goal in pathfinding is null."));
		return;
	}
	
}

