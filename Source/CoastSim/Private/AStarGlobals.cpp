// Fill out your copyright notice in the Description page of Project Settings.


#include "AStarGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "AStarNode.h"


// Sets default values
AAStarGlobals::AAStarGlobals()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAStarGlobals::BeginPlay()
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
void AAStarGlobals::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

TArray<AAStarNode*> AAStarGlobals::GetAStarNodes()
{
	return AStarNodes;
}

