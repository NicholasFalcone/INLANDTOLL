// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionPayload.h"
#include "BaseInteractable.h"
#include "InspectionProp.h"
#include "Components/SplineComponent.h"

// Sets default values
AInspectionPayload::AInspectionPayload()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CarMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CarMesh"));
	RootComponent = CarMesh;

	InteractablePosition = CreateDefaultSubobject<USceneComponent>(TEXT("InteractablePosition"));
	InteractablePosition->SetupAttachment(CarMesh);
}

// Called when the game starts or when spawned
void AInspectionPayload::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AInspectionPayload::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Muovi la macchina (la logica interna gestirà bIsMoving)
	MoveCar(DeltaTime);
}

void AInspectionPayload::MoveCar(float DeltaTime)
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

void AInspectionPayload::ResumeMovementToEnd()
{
	bIsMoving = true;
	StopAtSplineIndex = -1;
}

void AInspectionPayload::SetInspectionProp(TSubclassOf<AInspectionProp> PropClass, FString SocketName)
{
	if (PropClass && InteractablePosition)
	{
		AInspectionProp* InspectionInteraction = GetWorld()->SpawnActor<AInspectionProp>(PropClass, InteractablePosition->GetComponentLocation(), InteractablePosition->GetComponentRotation());
		if (!InspectionInteraction)
		{
			UE_LOG(LogTemp, Error, TEXT("ERRORE: Impossibile spawnare InspectionInteraction su %s!"), *GetName());
			return;
		}
		InspectionInteraction->AttachToComponent(CarMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, *SocketName);
	}
}

void AInspectionPayload::InitializeCarData(UStaticMesh* Mesh, FString SocketName, TSubclassOf<AInspectionProp> PropClass)
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

	CarMesh->SetStaticMesh(Mesh);
	SetInspectionProp(PropClass, SocketName);
	UE_LOG(LogTemp, Log, TEXT("Mesh impostata correttamente su %s"), *GetName());
}