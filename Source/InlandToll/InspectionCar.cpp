// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionCar.h"
#include "BaseInteractable.h"
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
	DoorInteraction->SetChildActorClass(ABaseInteractable::StaticClass());

	CargoInteraction = CreateDefaultSubobject<UChildActorComponent>(TEXT("CargoInteraction"));
	CargoInteraction->SetupAttachment(CarMesh, TEXT("CargoSocket"));
	CargoInteraction->SetChildActorClass(ABaseInteractable::StaticClass());

	InspectionInteraction = CreateDefaultSubobject<UChildActorComponent>(TEXT("InspectionInteraction"));
	InspectionInteraction->SetupAttachment(CarMesh, TEXT("InspectionSocket"));
	InspectionInteraction->SetChildActorClass(ABaseInteractable::StaticClass());
}

// Called when the game starts or when spawned
void AInspectionCar::BeginPlay()
{
	Super::BeginPlay();

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

	if (TargetSpline && DistanceAlongSpline < TargetSpline->GetSplineLength())
	{
		MoveCar(DeltaTime);
		bIsMoving = true;
	}
	else
	{
		bIsMoving = false;
	}
}

void AInspectionCar::MoveCar(float DeltaTime)
{
	if (!TargetSpline) return;

	float SplineLength = TargetSpline->GetSplineLength();
	float MaxDistance = SplineLength;

	// Se è impostato un indice di stop valido, calcola la distanza di quel punto
	if (StopAtSplineIndex >= 0 && StopAtSplineIndex < TargetSpline->GetNumberOfSplinePoints())
	{
		MaxDistance = TargetSpline->GetDistanceAlongSplineAtSplinePoint(StopAtSplineIndex);
	}
	
	if (DistanceAlongSpline < MaxDistance)
	{
		DistanceAlongSpline += movementSpeed * DeltaTime;
		if (DistanceAlongSpline > MaxDistance) DistanceAlongSpline = MaxDistance;

		FVector NewLocation = TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		FRotator NewRotation = TargetSpline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World);
		
		SetActorLocationAndRotation(NewLocation, NewRotation);
	}
}

void AInspectionCar::HandleDoorInteracted()
{
	if(bIsMoving)
	{
		UE_LOG(LogTemp, Warning, TEXT("Car is moving. Door interaction ignored."));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Delegate: Ricevuta interazione PORTA via ChildActor!"));
	bDoorOpen = !bDoorOpen;
}

void AInspectionCar::HandleCargoInteracted()
{
	if(bIsMoving)
	{
		UE_LOG(LogTemp, Warning, TEXT("Car is moving. Cargo interaction ignored."));
		return;
	}
	UE_LOG(LogTemp, Warning, TEXT("Delegate: Ricevuta interazione CARICO via ChildActor!"));
	bCargoOpen = !bCargoOpen;
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

void AInspectionCar::SetInspectionMesh(UStaticMesh* InspectionMesh, FString SocketName)
{
	if (InspectionMesh)
	{
		InspectionInteraction->AttachToComponent(CarMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, *SocketName);
		ABaseInteractable* InspectionActor = Cast<ABaseInteractable>(InspectionInteraction->GetChildActor());
		if(InspectionActor)
		{
			InspectionActor->MeshComp->SetStaticMesh(InspectionMesh);
		}
	}
}