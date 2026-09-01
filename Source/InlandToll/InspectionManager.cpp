// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "DialogueManagerSubsystem.h"

// Sets default values
AInspectionManager::AInspectionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("SplinePath"));
	RootComponent = SplinePath;
}

// Called when the game starts or when spawned
void AInspectionManager::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInspectionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInspectionManager::SpawnNextInspectionCar()
{
	if(CurrentInspectionCar)
	{
		UE_LOG(LogTemp, Warning, TEXT("An inspection car is already active. Cannot spawn a new one."));
		return;
	}


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

		CurrentInspectionCar = GetWorld()->SpawnActor<AInspectionPayload>(CarTemplate, SpawnLocation, SpawnRotation, SpawnParams);

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
		CurrentInspectionIndex = CurrentInspectionIndex % InspectionDataArray.Num(); // Wrap around if index exceeds array size

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

void AInspectionManager::RejectCurrentInspectionCar()
{
	if (CurrentInspectionCar)
	{
		// 1. Gather all attached child actors
		TArray<AActor*> AttachedActors;
		CurrentInspectionCar->GetAttachedActors(AttachedActors);
		// 2. Loop through and destroy each child
		for (AActor* ChildActor : AttachedActors)
		{
			if (IsValid(ChildActor))
			{
				ChildActor->Destroy();
			}
		}
		CurrentInspectionCar->Destroy();
		CurrentInspectionCar = nullptr;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No current inspection car to reject."));
	}
}

void AInspectionManager::PassCurrentInspectionDataToCar()
{
	if (CurrentInspectionCar)
	{
		CurrentInspectionCar->ResumeMovementToEnd();		
		CurrentInspectionCar->OnCarReachedEnd.AddDynamic(this, &AInspectionManager::HandleCarReachedEnd);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No current inspection car or invalid inspection data index."));
	}
}

void AInspectionManager::HandleCarReachedEnd(AInspectionPayload* Car)
{
	if (Car && IsValid(Car))
	{
		UE_LOG(LogTemp, Warning, TEXT("Car %s reached the end of the spline!"), *Car->GetName());
		Car->OnCarReachedEnd.RemoveAll(this);
		Car->Destroy();
		
		if (Car == CurrentInspectionCar)
		{
			if (InspectionDataArray[CurrentInspectionIndex]->InspectionData.bIsDangerous)
			{
				CurrentErrors++;
				if(CurrentErrors < MaxErrorsAllowed)
				{
					if(OnErrorCountChanged.IsBound())
					{
						OnErrorCountChanged.Broadcast(CurrentErrors);
					}
				}
				else
				{
					OnMaxErrorsReached.Broadcast();
				}
			}
			CurrentInspectionCar = nullptr;
		}
		SpawnNextInspectionCar(); 
	}
}

