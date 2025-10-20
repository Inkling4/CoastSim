// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AStarComponent.generated.h"

/*
 * This component will be used to store variables and functions for finding a path using A*.
 * TODO: Add a TGraph to store points for movement
 *
 */



UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class COASTSIM_API UAStarComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAStarComponent();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
