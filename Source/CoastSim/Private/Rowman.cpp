// Fill out your copyright notice in the Description page of Project Settings.


#include "Rowman.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputSubsystems.h"

// Sets default values
ARowman::ARowman()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ARowman::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARowman::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Copied from a lecture done in programming 2 (I take notes! >:3)
void ARowman::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(InputMapping, 0);
		}
	}
	if (UEnhancedInputComponent* Input = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		Input->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ARowman::LookInput);

	}
}

// Also copied from the same lecture from programming 2
void ARowman::LookInput(const FInputActionValue& InputValue)
{
	FVector2D InputVector = InputValue.Get<FVector2D>();

	if (IsValid(Controller))
	{
		AddControllerPitchInput(-InputVector.X);
		AddControllerYawInput(InputVector.Y);
	}
}
