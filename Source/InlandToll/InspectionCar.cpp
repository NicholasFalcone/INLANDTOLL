// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionCar.h"
#include "BaseInteractable.h"
#include "InspectionProp.h"
#include "Components/SplineComponent.h"

// Sets default values
AInspectionCar::AInspectionCar()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CarMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CarMesh"));
	RootComponent = CarMesh;

	DoorInteraction = CreateDefaultSubobject<UChildActorComponent>(TEXT("DoorInteraction"));
	DoorInteraction->SetupAttachment(CarMesh, TEXT("Door_LeftSocket"));

	CargoInteraction = CreateDefaultSubobject<UChildActorComponent>(TEXT("CargoInteraction"));
	CargoInteraction->SetupAttachment(CarMesh, TEXT("CargoSocket"));

	InspectionInteraction = CreateDefaultSubobject<UChildActorComponent>(TEXT("InspectionInteraction"));
	InspectionInteraction->SetupAttachment(CarMesh, TEXT("InspectionSocket"));
}

// Called when the game starts or when spawned
void AInspectionCar::BeginPlay()
{
	Super::BeginPlay();

	// Initialize child actor classes if not already set (e.g. from Blueprint defaults)
	// Setting these here is safer than in the constructor.
	if (DoorInteraction && !DoorInteraction->GetChildActorClass())
	{
		DoorInteraction->SetChildActorClass(ABaseInteractable::StaticClass());
	}
	if (CargoInteraction && !CargoInteraction->GetChildActorClass())
	{
		CargoInteraction->SetChildActorClass(ABaseInteractable::StaticClass());
	}
	if (InspectionInteraction && !InspectionInteraction->GetChildActorClass())
	{
		InspectionInteraction->SetChildActorClass(ABaseInteractable::StaticClass());
	}

	if (DoorInteraction && DoorInteraction->GetChildActor())
	{
		if (ABaseInteractable* DoorActor = Cast<ABaseInteractable>(DoorInteraction->GetChildActor()))
		{
			DoorActor->OnInteractDelegate.AddDynamic(this, &AInspectionCar::HandleDoorInteracted);
		}
	}

	if (CargoInteraction && CargoInteraction->GetChildActor())
	{
		if (ABaseInteractable* CargoActor = Cast<ABaseInteractable>(CargoInteraction->GetChildActor()))
		{
			CargoActor->OnInteractDelegate.AddDynamic(this, &AInspectionCar::HandleCargoInteracted);
		}
	}

	if (InspectionInteraction && InspectionInteraction->GetChildActor())
	{
		if (ABaseInteractable* InspectionActor = Cast<ABaseInteractable>(InspectionInteraction->GetChildActor()))
		{
			InspectionActor->OnInteractDelegate.AddDynamic(this, &AInspectionCar::HandleInspectionInteracted);
		}
	}
}


// Called every frame
void AInspectionCar::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Muovi la macchina (la logica interna gestirà bIsMoving)
	MoveCar(DeltaTime);
}

void AInspectionCar::MoveCar(float DeltaTime)
{
	if (!TargetSpline) 
	{
		bIsMoving = false;
		return;
	}

	float SplineLength = TargetSpline->GetSplineLength();
	float MaxDistance = SplineLength;

	// Se è impostato un indice di stop valido, calcola la distanza di quel punto
	if (StopAtSplineIndex >= 0 && StopAtSplineIndex < TargetSpline->GetNumberOfSplinePoints())
	{
		MaxDistance = TargetSpline->GetDistanceAlongSplineAtSplinePoint(StopAtSplineIndex);
	}
	
	if (DistanceAlongSpline < MaxDistance)
	{
		bIsMoving = true;
		DistanceAlongSpline += movementSpeed * DeltaTime;
		
		if (DistanceAlongSpline > MaxDistance) 
		{
			DistanceAlongSpline = MaxDistance;
		}

		FVector NewLocation = TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		FRotator NewRotation = TargetSpline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		
		SetActorLocationAndRotation(NewLocation, NewRotation);
	}
	else
	{
		// Siamo fermi (o allo stop intermedio o alla fine)
		bIsMoving = false;

		// Se siamo fermi alla fine assoluta, lancia l'evento una sola volta
		if (DistanceAlongSpline >= SplineLength && !bHasReachedEnd)
		{
			bHasReachedEnd = true;
			OnCarReachedEnd.Broadcast(this);
		}
	}
}

void AInspectionCar::ResumeMovementToEnd()
{
	bIsMoving = true;
	bCargoOpen = false;
	bDoorOpen = false;	
	StopAtSplineIndex = -1;
}

void AInspectionCar::HandleDoorInteracted()
{
	if(bIsMoving)
	{
		UE_LOG(LogTemp, Warning, TEXT("Car is moving. Door interaction ignored."));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Delegate: Ricevuta interazione PORTA via ChildActor!"));

	if(bDoorOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("Porta aperta. Chiudendo la porta."));
	}
	else
	{
		bDoorOpen = true;
		DoorInteraction->GetChildActor()->SetActorEnableCollision(false); // Nascondi il ChildActor della porta
		UE_LOG(LogTemp, Warning, TEXT("Porta chiusa. Aprendo la porta."));
	}
}

void AInspectionCar::HandleCargoInteracted()
{
	if(bIsMoving)
	{
		UE_LOG(LogTemp, Warning, TEXT("Car is moving. Cargo interaction ignored."));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Delegate: Ricevuta interazione CARICO via ChildActor!"));
	if(bCargoOpen)
	{
		UE_LOG(LogTemp, Warning, TEXT("Carico aperto. Chiudendo il carico."));
	}
	else
	{
		bCargoOpen = true;
		CargoInteraction->GetChildActor()->SetActorEnableCollision(false); // Nascondi il ChildActor del carico
		UE_LOG(LogTemp, Warning, TEXT("Carico chiuso. Aprendo il carico."));
	}
}

void AInspectionCar::HandleInspectionInteracted()
{
	if(bIsMoving)
	{
		UE_LOG(LogTemp, Warning, TEXT("Car is moving. Inspection interaction ignored."));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Delegate: Ricevuta interazione ISPEZIONE via ChildActor!"));
}

void AInspectionCar::SetInspectionProp(TSubclassOf<AInspectionProp> PropClass, FString SocketName)
{
	if (PropClass && InspectionInteraction)
	{
		InspectionInteraction->SetChildActorClass(PropClass);
		InspectionInteraction->AttachToComponent(CarMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, *SocketName);
	}
}

void AInspectionCar::InitializeCarData(USkeletalMesh* Mesh, FString SocketName, TSubclassOf<AInspectionProp> PropClass)
{
	if (!CarMesh)
	{
		UE_LOG(LogTemp, Error, TEXT("ERRORE: CarMesh component è NULL su %s! Il Blueprint potrebbe essere corrotto."), *GetName());
		return;
	}

	if (!Mesh)
	{
		UE_LOG(LogTemp, Warning, TEXT("ATTENZIONE: Mesh passata a InitializeCarData è NULL su %s"), *GetName());
		return;
	}

	CarMesh->SetSkeletalMesh(Mesh);
	SetInspectionProp(PropClass, SocketName);
	UE_LOG(LogTemp, Log, TEXT("Mesh impostata correttamente su %s"), *GetName());
}