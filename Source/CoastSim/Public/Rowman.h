// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Rowman.generated.h"

struct FInputActionValue; // Forward declaration

UCLASS()
class COASTSIM_API ARowman : public ACharacter
{
	GENERATED_BODY()
	
public:
	// Sets default values for this character's properties
	ARowman();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, category = "Input") // The input mapping context
	class UInputMappingContext* InputMapping;
	UPROPERTY(EditDefaultsOnly, category = "Input") // Input for looking around with the mouse
	class UInputAction* IA_Look; 

	void LookInput(const FInputActionValue& InputValue);

	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
