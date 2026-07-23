// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionCarManager.h"

// Sets default values
AInspectionCarManager::AInspectionCarManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StartingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("StartingPoint"));
	StartingPoint->SetupAttachment(RootComponent);

	EndingPoint = CreateDefaultSubobject<USceneComponent>(TEXT("EndingPoint"));
	EndingPoint->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AInspectionCarManager::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AInspectionCarManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AInspectionCarManager::SpawnNextInspectionCar()
{
	if (CurrentInspectionIndex < InspectionDataArray.Num())
	{
		UInspectionCarDataAsset* CurrentInspectionData = InspectionDataArray[CurrentInspectionIndex];

		// Spawn the inspection car
		FActorSpawnParameters SpawnParams;
		AInspectionCar* NewCar = GetWorld()->SpawnActor<AInspectionCar>(AInspectionCar::StaticClass(), StartingPoint->GetComponentLocation(), FRotator::ZeroRotator, SpawnParams);

		if (NewCar)
		{
			NewCar->InitializeCarData(CurrentInspectionData->CarMesh, CurrentInspectionData->InspectionData.AttachedSocketName, CurrentInspectionData->InspectionData.InspectionMesh);
			NewCar->InitializeCarMovement(StartingPoint->GetComponentLocation(), EndingPoint->GetComponentLocation());
			InspectionCars = NewCar;
		}

		CurrentInspectionIndex++;
	}
}

