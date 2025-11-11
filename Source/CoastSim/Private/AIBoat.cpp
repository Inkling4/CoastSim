// Fill out your copyright notice in the Description page of Project Settings.


#include "AIBoat.h"

// Sets default values
AAIBoat::AAIBoat()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AAIBoat::BeginPlay()
{
	Super::BeginPlay();

	
}


// Called every frame
void AAIBoat::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AAIBoat::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

