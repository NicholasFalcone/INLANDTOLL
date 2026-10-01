// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "DialogueManagerSubsystem.h"
#include "Anomaly/InspectableAnomaly.h"
#include "Incenerator.h"
#include "InspectionPayload.h"
#include "Tools/Tablet.h"
#include "Variant_Horror/HorrorCharacter.h"

// Sets default values
AInspectionManager::AInspectionManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("SplinePath"));
	RootComponent = SplinePath;
}

void AInspectionManager::HandlePlayerDied()
{
	UE_LOG(LogTemp, Warning, TEXT("Player died. Handling inspection car destruction."));
	DestroyAllAnomaly();
}

void AInspectionManager::DestroyAllAnomaly()
{
	DestroyCurrentInspection();
	/// Find all Actor of class Anomaly
	TArray<AActor*> FoundAnomalies;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInspectableAnomaly::StaticClass(), FoundAnomalies);
	for (AActor* Anomaly : FoundAnomalies)
	{
		if (Anomaly)
		{
			Anomaly->Destroy();
		}
	}
}

// Called when the game starts or when spawned
void AInspectionManager::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<AHorrorCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
	if(PlayerCharacter)
	{
		PlayerCharacter->OnPlayerDied.AddDynamic(this, &AInspectionManager::HandlePlayerDied);
	}
}

// Called every frame
void AInspectionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInspectionManager::SpawnNextAnomaly()
{
	if(CurrentDayInspection >= InspectionToDailyReach)
	{
		UE_LOG(LogTemp, Warning, TEXT("Daily inspection limit reached. Cannot spawn more inspection cars."));
		if(OnDailyInspectionLimitReached.IsBound())
		{
			OnDailyInspectionLimitReached.Broadcast();
		}
		return;
	}

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
			CurrentInspectionCar->bIsDangerous = CurrentInspectionData->InspectionData.bIsDangerous;
			CurrentInspectionCar->InitializeCarData(CurrentCarMesh, CurrentInspectionData->InspectionData.AttachedSocketName, CurrentInspectionData->InspectionData.InspectionPropClass);
			CurrentInspectionCar->InitializeCarMovement(SplinePath);

			// Trigger dialogue associated with this anomaly comming up for inspection
			if (UDialogueManagerSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueManagerSubsystem>())
			{
				DialogueSubsystem->PlayDialogueSequence(CurrentInspectionData->InspectionData.InspectionDialogueLines);
			}
		}
		CurrentInspectionIndex++;
		CurrentInspectionIndex = CurrentInspectionIndex % InspectionDataArray.Num(); // Wrap around if index exceeds array size

		if(OnDailyInspectionCountChanged.IsBound())
		{
			OnDailyInspectionCountChanged.Broadcast(CurrentDayInspection);
		}

		if(!TabletInstance)
		{
			if (PlayerCharacter)
			{
				TabletInstance = PlayerCharacter->MyTablet;
			}
			
			if(!TabletInstance)
			{
				UE_LOG(LogTemp, Warning, TEXT("Tablet instance still not found in the player. Cannot update anomaly."));
				return;
			}
		}

		TabletInstance->UpdateAnomaly(CurrentInspectionData->InspectionData);
	}
}

void AInspectionManager::RejectCurrentInspectedAnomaly()
{
	if (CurrentInspectionCar)
	{
		if (AIncenerator* Incenerator = Cast<AIncenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AIncenerator::StaticClass())))
		{
			Incenerator->StartInceneratorSequence();
		}
		else
		{
			// Fallback if no Incenerator in level
			DestroyCurrentInspection();
		}
		CurrentDayInspection++;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No current inspection car to reject."));
	}
}

void AInspectionManager::DestroyCurrentInspection(){
	if (AIncenerator* Incenerator = Cast<AIncenerator>(UGameplayStatics::GetActorOfClass(GetWorld(), AIncenerator::StaticClass())))
	{
		AActor* FoundAnomaly = nullptr;
		TArray<AActor*> AttachedActors;
		CurrentInspectionCar->GetAttachedActors(AttachedActors);
		for (AActor* Actor : AttachedActors)
		{
			if (Actor && Actor->IsA(AInspectionProp::StaticClass()))
			{
				FoundAnomaly = Actor;
				break;
			}
		}
		if (FoundAnomaly)
		{
			Incenerator->BurnAnomaly(FoundAnomaly);
		}
	}
}

void AInspectionManager::PassCurrentInspectionDataToAnomaly()
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
		// UE_LOG(LogTemp, Warning, TEXT("Car %s reached the end of the spline!"), *Car->GetName());
		Car->OnCarReachedEnd.RemoveAll(this);
		
		if (Car == CurrentInspectionCar)
		{
			if (!Car->bIsRejected && Car->bIsDangerous)
			{
				// Trigger dialogue associated with this anomaly comming up for inspection
				if (UDialogueManagerSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueManagerSubsystem>())
				{
					if (ErrorDialogueLines.Num() > 0)
					{
						int32 index = FMath::Clamp(CurrentErrors, 0, ErrorDialogueLines.Num() - 1);
						DialogueSubsystem->PlayDialogue(ErrorDialogueLines[index]);
					}
				}
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
		CurrentDayInspection++;
		Car->Destroy();
		SpawnNextAnomaly(); 
	}
}


void AInspectionManager::RestartGame()
{
	CurrentErrors = 0;
	CurrentDayInspection = 0;
	CurrentInspectionIndex = 0;

	if(OnErrorCountChanged.IsBound())
	{
		OnErrorCountChanged.Broadcast(CurrentErrors);
	}
	if(OnDailyInspectionCountChanged.IsBound())
	{
		OnDailyInspectionCountChanged.Broadcast(CurrentDayInspection);
	}
	if(CurrentInspectionCar)
	{
		CurrentInspectionCar->Destroy();
		CurrentInspectionCar = nullptr;
	}
	SpawnNextAnomaly();
}
