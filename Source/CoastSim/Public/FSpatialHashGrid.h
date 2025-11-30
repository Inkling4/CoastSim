// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DrawDebugHelpers.h"
#include "Templates/UniquePtr.h"
#include "HAL/CriticalSection.h"
#include "Containers/Map.h"
#include "FSpatialHashGrid.generated.h"

USTRUCT()
struct FSpatialHashGrid
{

	GENERATED_BODY()
	
	FSpatialHashGrid(float CellSize, bool bUseNeighborCells);
	FSpatialHashGrid() = default;

private:
	
	TMap<FIntVector, TArray<int32>> Cells;

public:
	UPROPERTY(EditAnywhere, Category="Grid Settings")
	float CellSize;
	
	bool bUseNeighborCells = true;

	//Clears the grid of all boids 
	void ClearGrid();
	
	FIntVector GetCellVector(const FVector& Position) const;


	void InsertBoid(int32 BoidIndex, const FIntVector& Cell);
	
	void RemoveBoid(int32 BoidIndex, const FIntVector& Cell);

	bool HasChangedCell(const FVector& OldPosition, const FVector&NewPosition) const;
	
	void GetNeighbourBoids(const FVector& Position, TArray<int32>& OutIndices) const;

	void DrawGrid(UWorld* World, const FColor& Color = FColor::Green, float Duration = 0.0f) const;

};
