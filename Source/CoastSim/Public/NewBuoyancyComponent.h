// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "OceanWaterZone.h"

#include "NewBuoyancyComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COASTSIM_API UNewBuoyancyComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UNewBuoyancyComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Data")
	TArray<FVector> PontoonsLocations = { FVector::ZeroVector };
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Data")
	float RotationStrength = 5;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Data")
	bool DebugPoints = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Buoyancy Data")
	UStaticMeshComponent* MyStaticMeshComponent;

	UFUNCTION(BlueprintCallable, Category = "Buoyancy")
	FVector GetBuoyancyLocation(FVector RelativeLocation);
	UFUNCTION(BlueprintCallable, Category = "Buoyancy")
	FVector GetMultiBuoyancyLocation(TArray<FVector> PontoonsArray);
	UFUNCTION(BlueprintCallable, Category = "Buoyancy")
	TArray<FVector> GetBuoyancyArray(TArray<FVector> Points);

	UPROPERTY(EditAnywhere, BlueprintReadWrite) int YPoints = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) int XPoints = 3;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ObjectLengthY = 1000;
	UPROPERTY(EditAnywhere, BlueprintReadWrite) float ObjectLengthX = 300;

private:
	AActor* ParentActor = nullptr;
	AOceanWaterZone* OceanWaterZone;
	FTransform ActorTransform;
	FVector WorldActorLocation;
	FRotator WorldActorRotation;
	FVector RelativeStaticMeshLocation;
	FRotator RelativeStaticMeshRotation;
	FOceanFFTCalculator* FFTCalculator;
	bool bWaterZoneValid = true;

	FOceanFFTCalculator* InitializeWaterZoneReference();
	FVector FindAverageLocation(TArray<FVector> Locations);
	FQuat CalculateBuoyancyRotation(const TArray<FVector> Points);
	FQuat CalculateWaveRotation(const FVector& WavePoint);
	void DrawBuoyancyArrayDebugPoints(const TArray<FVector>& array);

	FRotator prevTarget;
	TArray<FVector> BuoyancyArray = { FVector::ZeroVector };
	float BoatPointOffsetValue(FVector point);

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
};
