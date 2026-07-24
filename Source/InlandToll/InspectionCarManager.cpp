// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionCarManager.h"
#include "Components/SplineComponent.h"

// Sets default values
AInspectionCarManager::AInspectionCarManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("SplinePath"));
	RootComponent = SplinePath;
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

	if(CurrentInspectionCar)
	{
		UE_LOG(LogTemp, Warning, TEXT("An inspection car is already active. Cannot spawn a new one."));
		return;
	}

	CurrentInspectionIndex = CurrentInspectionIndex % InspectionDataArray.Num(); // Wrap around if index exceeds array size

	if (CurrentInspectionIndex < InspectionDataArray.Num())
	{
		UInspectionCarDataAsset* CurrentInspectionData = InspectionDataArray[CurrentInspectionIndex];
		USkeletalMesh* CurrentCarMesh = CarMeshes[FMath::RandRange(0, CarMeshes.Num() - 1)]; // Randomly select a car mesh from the array
		// Spawn the inspection car
		FActorSpawnParameters SpawnParams;
		FVector SpawnLocation = SplinePath->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
		FRotator SpawnRotation = SplinePath->GetRotationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);

		CurrentInspectionCar = GetWorld()->SpawnActor<AInspectionCar>(AInspectionCar::StaticClass(), SpawnLocation, SpawnRotation, SpawnParams);

		if (CurrentInspectionCar)
		{
			CurrentInspectionCar->InitializeCarData(CurrentCarMesh, CurrentInspectionData->InspectionData.AttachedSocketName, CurrentInspectionData->InspectionData.InspectionMesh);
			CurrentInspectionCar->InitializeCarMovement(SplinePath);
		}
		CurrentInspectionIndex++;
	}
}

void AInspectionCarManager::PassCurrentInspectionDataToCar()
{
	if (CurrentInspectionCar)
	{
		CurrentInspectionCar->ResumeMovementToEnd();		
		CurrentInspectionCar->OnCarReachedEnd.AddDynamic(this, &AInspectionCarManager::HandleCarReachedEnd);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No current inspection car or invalid inspection data index."));
	}
}

void AInspectionCarManager::HandleCarReachedEnd(AInspectionCar* Car)
{
	if (Car && IsValid(Car))
	{
		UE_LOG(LogTemp, Warning, TEXT("Car %s reached the end of the spline!"), *Car->GetName());
		Car->OnCarReachedEnd.RemoveAll(this);
		Car->Destroy();
		
		if (Car == CurrentInspectionCar)
		{
			CurrentInspectionCar = nullptr;
		}

		SpawnNextInspectionCar(); 
	}
}

