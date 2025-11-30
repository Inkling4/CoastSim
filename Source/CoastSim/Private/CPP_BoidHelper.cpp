// Fill out your copyright notice in the Description page of Project Settings.


#include "CPP_BoidHelper.h"

TArray<FVector> FCPP_BoidHelper::Directions;


void FCPP_BoidHelper::Init()
{
	if (Directions.Num() == 0)
	{
		Directions.SetNum(NumViewDirections);

		const float GoldenRatio = (1.f + FMath::Sqrt(5.f)) * 0.5f;
		const float AngleIncrement = 2.f * PI * GoldenRatio;

		for (int32 i = 0; i < NumViewDirections; i++)
		{
			float t = static_cast<float>(i) / NumViewDirections;
			float Inclination = FMath::Acos(1.f - 2.f * t);
			float Azimuth = AngleIncrement * i;

			float z = FMath::Sin(Inclination) * FMath::Cos(Azimuth);
			float y = FMath::Sin(Inclination) * FMath::Sin(Azimuth);
			float x = FMath::Cos(Inclination);

			Directions[i] = FVector(x, y, z);
		}
	}
}
