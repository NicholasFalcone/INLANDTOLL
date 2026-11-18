// Fill out your copyright notice in the Description page of Project Settings.

#include "ToolPlacementManager.h"
#include "Tool.h"
#include "ToolPlacementPoint.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "EngineUtils.h"
#include "DrawDebugHelpers.h"

UToolPlacementManager::UToolPlacementManager()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f; // Update every 0.1 seconds to reduce overhead
}

void UToolPlacementManager::BeginPlay()
{
	Super::BeginPlay();
}

void UToolPlacementManager::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Update which placement point is highlighted
	UpdateHighlightedPlacementPoint();
}

bool UToolPlacementManager::TryPlaceCurrentTool()
{
	if (!EquippedTool || !HighlightedPlacementPoint)
	{
		return false;
	}

	if (HighlightedPlacementPoint->TryPlaceTool(EquippedTool))
	{
		EquippedTool = nullptr;
		return true;
	}

	return false;
}

AATool* UToolPlacementManager::TryPickupToolFromPlacement()
{
	if (!HighlightedPlacementPoint)
	{
		return nullptr;
	}

	AATool* PickedUpTool = HighlightedPlacementPoint->RemoveTool();
	if (PickedUpTool)
	{
		SetEquippedTool(PickedUpTool);
	}

	return PickedUpTool;
}

void UToolPlacementManager::UpdateHighlightedPlacementPoint()
{
	// Try to get the placement point the player is looking at
	AToolPlacementPoint* NewHighlightedPoint = GetLookAtPlacementPoint();

	// If not looking at one, find the closest one within radius
	if (!NewHighlightedPoint)
	{
		NewHighlightedPoint = FindClosestPlacementPoint();
	}

	// Update highlighting
	if (NewHighlightedPoint != PreviousHighlightedPoint)
	{
		if (PreviousHighlightedPoint)
		{
			PreviousHighlightedPoint->Unhighlight();
		}

		PreviousHighlightedPoint = NewHighlightedPoint;
		HighlightedPlacementPoint = NewHighlightedPoint;

		if (NewHighlightedPoint)
		{
			NewHighlightedPoint->Highlight();
		}
	}
}

void UToolPlacementManager::SetEquippedTool(AATool* NewTool)
{
	EquippedTool = NewTool;
}

void UToolPlacementManager::GetAllPlacementPoints(TArray<AToolPlacementPoint*>& OutPlacementPoints)
{
	OutPlacementPoints.Empty();

	for (TActorIterator<AToolPlacementPoint> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		OutPlacementPoints.Add(*ActorItr);
	}
}

AToolPlacementPoint* UToolPlacementManager::FindClosestPlacementPoint()
{
	TArray<AToolPlacementPoint*> PlacementPoints;
	GetAllPlacementPoints(PlacementPoints);

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return nullptr;
	}

	AToolPlacementPoint* ClosestPoint = nullptr;
	float ClosestDistance = InteractionRadius;

	for (AToolPlacementPoint* Point : PlacementPoints)
	{
		if (!Point || !Point->bIsActive)
		{
			continue;
		}

		float Distance = FVector::Dist(Owner->GetActorLocation(), Point->GetActorLocation());
		if (Distance < ClosestDistance)
		{
			ClosestDistance = Distance;
			ClosestPoint = Point;
		}
	}

	return ClosestPoint;
}

AToolPlacementPoint* UToolPlacementManager::GetLookAtPlacementPoint()
{
	AActor* Owner = GetOwner();
	APawn* OwnerPawn = Cast<APawn>(Owner);

	if (!Owner || !OwnerPawn)
	{
		return nullptr;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC)
	{
		return nullptr;
	}

	FVector CameraLocation = Owner->GetActorLocation();
	FVector CameraForward = Owner->GetActorForwardVector();

	// Get camera location if available
	if (OwnerPawn->FindComponentByClass<class UCameraComponent>())
	{
		UCameraComponent* CameraComp = OwnerPawn->FindComponentByClass<class UCameraComponent>();
		CameraLocation = CameraComp->GetComponentLocation();
		CameraForward = CameraComp->GetForwardVector();
	}

	FVector TraceEnd = CameraLocation + (CameraForward * InteractionRadius);
	FHitResult HitResult;

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Owner);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		CameraLocation,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	if (bHit && HitResult.GetActor())
	{
		AToolPlacementPoint* PlacementPoint = Cast<AToolPlacementPoint>(HitResult.GetActor());
		if (PlacementPoint)
		{
			return PlacementPoint;
		}
	}

	return nullptr;
}
