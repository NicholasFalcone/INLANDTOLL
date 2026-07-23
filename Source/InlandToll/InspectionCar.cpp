// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionCar.h"

// Sets default values
AInspectionCar::AInspectionCar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CarMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CarMesh"));
	RootComponent = CarMesh;
}

// Called when the game starts or when spawned
void AInspectionCar::BeginPlay()
{
	Super::BeginPlay();
	startLocation = GetActorLocation();
}

// Called every frame
void AInspectionCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	MoveCar(DeltaTime);
}

void AInspectionCar::MoveCar(float DeltaTime)
{
	FVector CurrentLocation = GetActorLocation();
	FVector Direction = (endLocation - startLocation).GetSafeNormal();
	FVector NewLocation = CurrentLocation + Direction * movementSpeed * DeltaTime;
	// Check if the car has reached or passed the end location
	if (FVector::Dist(NewLocation, startLocation) >= FVector::Dist(endLocation, startLocation))
	{
		NewLocation = endLocation; // Snap to end location
	}
	SetActorLocation(NewLocation);
}