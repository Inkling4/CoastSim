#pragma once

#include "WaterZoneActor.h"
#include "OceanFFTCalculator.h"

#include "OceanWaterZone.generated.h"

UCLASS(BlueprintType)
class AOceanWaterZone : public AWaterZone
{
    GENERATED_BODY()

    AOceanWaterZone(const FObjectInitializer& ObjectInitializer);

public:

    virtual void Tick(float DeltaSeconds) override;
    bool ShouldTickIfViewportsOnly() const;

    //void UpdatePosition(FVector NewLocation);

    FOceanFFTCalculator FFTCalculator;
};
