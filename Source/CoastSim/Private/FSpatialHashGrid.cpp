// Fill out your copyright notice in the Description page of Project Settings.

#include "FSpatialHashGrid.h"

FIntVector FSpatialHashGrid::GetCellVector(const FVector& Position) const
{
	return FIntVector(
	FMath::FloorToInt(Position.X / CellSize),
	FMath::FloorToInt(Position.Y / CellSize),
	FMath::FloorToInt(Position.Z / CellSize)
	);
}

FSpatialHashGrid::FSpatialHashGrid(float CellSize, bool bUseNeighborCells)
{
	this->CellSize = CellSize;
	this->bUseNeighborCells = bUseNeighborCells;
	Cells.Reserve(255);
}

void FSpatialHashGrid::ClearGrid()
{
	Cells.Empty();
	
}

void FSpatialHashGrid::InsertBoid(int32 BoidIndex, const FIntVector& Cell)
{
	Cells.FindOrAdd(Cell).Add(BoidIndex);
}

void FSpatialHashGrid::RemoveBoid(int32 BoidIndex, const FIntVector& Cell)
{
	if (TArray<int32>* CellBoids = Cells.Find(Cell))
	{
		CellBoids->Remove(BoidIndex);
		
	}
}

bool FSpatialHashGrid::HasChangedCell(const FVector& OldPosition, const FVector& NewPosition) const
{
	if (GetCellVector(OldPosition) != GetCellVector(NewPosition))
	{
		return true;
	}
	return false;
}

void FSpatialHashGrid::GetNeighbourBoids(const FVector& Position, TArray<int32>& OutIndices) const
{

	OutIndices.Empty();

	const FIntVector CenterCell = GetCellVector(Position);

	if (bUseNeighborCells)
	{
		for (int32 x = -1; x <= 1; x++)
		{
			for (int32 y = -1; y <= 1; y++)
			{
				for (int32 z = -1; z <= 1; z++)
				{
					FIntVector NeighborCell = CenterCell + FIntVector(x, y, z);
					if (const TArray<int32>* CellBoids = Cells.Find(NeighborCell))
					{
						OutIndices.Append(*CellBoids);
					}
				}
			}
		}
	}
	else
	{
		if (const TArray<int32>* CellBoids = Cells.Find(CenterCell))
		{
			OutIndices.Append(*CellBoids);
		}
	}
}

void FSpatialHashGrid::DrawGrid(UWorld* World, const FColor& Color, float Duration) const
{
	if (!World) return;

	for (const TPair<FIntVector, TArray<int32>>& CellPair : Cells)
	{
		const FIntVector& CellCoords = CellPair.Key;

		FVector Center = FVector(
			(CellCoords.X + 0.5f) * CellSize,
			(CellCoords.Y + 0.5f) * CellSize,
			(CellCoords.Z + 0.5f) * CellSize
		);

		DrawDebugBox(
			World,
			Center,
			FVector(CellSize * 0.5f),
			Color,
			false,
			Duration,
			0,
			2.0f
		);
	}
	
}
