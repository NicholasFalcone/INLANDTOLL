// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionCarManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "DialogueManagerSubsystem.h"

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
		UStaticMesh* CurrentCarMesh = CarMeshes[FMath::RandRange(0, CarMeshes.Num() - 1)]; // Randomly select a car mesh from the array
		// Spawn the inspection car
		FActorSpawnParameters SpawnParams;
		FVector SpawnLocation = SplinePath->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);
		FRotator SpawnRotation = SplinePath->GetRotationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World);

		if(!CarTemplate)
		{
			UE_LOG(LogTemp, Error, TEXT("CarTemplate is not set! Cannot spawn inspection car."));
			return;
		}

		CurrentInspectionCar = GetWorld()->SpawnActor<AInspectionCar>(CarTemplate, SpawnLocation, SpawnRotation, SpawnParams);

		if (CurrentInspectionCar)
		{
			CurrentInspectionCar->InitializeCarData(CurrentCarMesh, CurrentInspectionData->InspectionData.AttachedSocketName, CurrentInspectionData->InspectionData.InspectionPropClass);
			CurrentInspectionCar->InitializeCarMovement(SplinePath);

			// Trigger dialogue associated with this car/inspection
			if (UDialogueManagerSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueManagerSubsystem>())
			{
				DialogueSubsystem->PlayDialogueSequence(CurrentInspectionData->InspectionData.InspectionDialogueLines);
			}
		}
		CurrentInspectionIndex++;
		if(!TabletInstance)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tablet instance not found in the world. Cannot update anomaly."));
			TabletInstance = Cast<ATablet>(UGameplayStatics::GetActorOfClass(GetWorld(), ATablet::StaticClass()));
			if(!TabletInstance){
				UE_LOG(LogTemp, Warning, TEXT("Tablet instance still not found in the world. Cannot update anomaly."));
				return;
			}
			TabletInstance->UpdateAnomaly(CurrentInspectionData->InspectionData);
			return;
		}

		TabletInstance->UpdateAnomaly(CurrentInspectionData->InspectionData);
	}
}

void AInspectionCarManager::RejectCurrentInspectionCar()
{
	if (CurrentInspectionCar)
	{
		CurrentInspectionCar->Destroy();
		CurrentInspectionCar = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No current inspection car to reject."));
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

