#pragma once

#include "CoreMinimal.h"
#include "CPP_BoidHelper.generated.h"



USTRUCT()
struct FCPP_BoidHelper
{
	GENERATED_BODY()

	static const int32 NumViewDirections = 64;
	static TArray<FVector> Directions;
	
	static void Init();
	
};
