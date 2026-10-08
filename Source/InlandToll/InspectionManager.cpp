// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionManager.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "DialogueManagerSubsystem.h"
#include "Anomaly/InspectableAnomaly.h"
#include "Incenerator.h"
#include "InspectionPayload.h"
#include "Desk.h"
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
	DeskInstance = Cast<ADesk>(UGameplayStatics::GetActorOfClass(GetWorld(), ADesk::StaticClass()));
}

// Called every frame
void AInspectionManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AInspectionManager::SpawnNextAnomaly()
{
	UE_LOG(LogTemp, Warning, TEXT("Spawning next anomaly."));

	if(CurrentInspectionPayload)
	{
		UE_LOG(LogTemp, Warning, TEXT("An inspection car is already active. Cannot spawn a new one."));
		return;
	}
	
	if(DeskInstance == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("DeskInstance is not set. Cannot spawn next anomaly."));
		return;
	}

	bool shouldSpawnNextAnomaly = !DeskInstance->IsDayOver(CurrentInspectionIndex);
	UE_LOG(LogTemp, Warning, TEXT("Should spawn next anomaly: %s"), shouldSpawnNextAnomaly ? TEXT("true") : TEXT("false"));
	if (shouldSpawnNextAnomaly)
	{
		UAnomalyData* CurrentInspectionData = DeskInstance->GetInspectionDataForCurrentDay(CurrentInspectionIndex);
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

		CurrentInspectionPayload = GetWorld()->SpawnActor<AInspectionPayload>(CarTemplate, SpawnLocation, SpawnRotation, SpawnParams);

		if (CurrentInspectionPayload)
		{
			CurrentInspectionPayload->bIsDangerous = CurrentInspectionData->bIsDangerous;
			if(CurrentInspectionData->InspectionPropClass != nullptr)
			{
				CurrentInspectionPayload->InitializeInspectableAnomaly(CurrentCarMesh, CurrentInspectionData->AttachedSocketName, CurrentInspectionData->InspectionPropClass);
			}
			else if(CurrentInspectionData->InspectionAnomalyClass != nullptr)
			{
				CurrentInspectionPayload->InitializeStaticAnomaly(CurrentCarMesh, CurrentInspectionData->AttachedSocketName, CurrentInspectionData->InspectionAnomalyClass);
			}
			CurrentInspectionPayload->InitializeSplineMovement(SplinePath);

			// Trigger dialogue associated with this anomaly comming up for inspection
			if (UDialogueManagerSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueManagerSubsystem>())
			{
				DialogueSubsystem->PlayDialogueSequence(CurrentInspectionData->InspectionDialogueLines);
			}
		}
		CurrentInspectionIndex++;

		OnAnomalyDetailsChange.Broadcast(CurrentInspectionData);
	}
}

void AInspectionManager::RejectCurrentInspectedAnomaly()
{
	if (CurrentInspectionPayload)
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
		CurrentInspectionPayload->GetAttachedActors(AttachedActors);
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
	if (CurrentInspectionPayload)
	{
		CurrentInspectionPayload->ResumeMovementToEnd();		
		CurrentInspectionPayload->OnCarReachedEnd.AddDynamic(this, &AInspectionManager::HandleCarReachedEnd);
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
		
		if (Car == CurrentInspectionPayload)
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
				if(OnInspectionError.IsBound())
				{
					OnInspectionError.Broadcast();
				}

			}
			CurrentInspectionPayload = nullptr;
		}

		if(OnInspectionEnded.IsBound())
		{
			OnInspectionEnded.Broadcast();
		}
		
		Car->Destroy();
		SpawnNextAnomaly(); 
	}
}

void AInspectionManager::RestartGame()
{
	CurrentInspectionIndex = 0;
	CurrentErrors = 0;
	if(CurrentInspectionPayload)
	{
		CurrentInspectionPayload->Destroy();
		CurrentInspectionPayload = nullptr;
	}
	SpawnNextAnomaly();
}
