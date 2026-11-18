// Fill out your copyright notice in the Description page of Project Settings.

#include "SimplePlacementPoint.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"
#include "DrawDebugHelpers.h"

ASimplePlacementPoint::ASimplePlacementPoint()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ASimplePlacementPoint::ApplyPreset(EPlacementPreset Preset)
{
	switch (Preset)
	{
		case EPlacementPreset::EPS_ThermometerStand:
			AcceptedToolType = EToolType::ETT_Thermometer;
			PlacementLabel = FText::FromString(TEXT("Thermometer Stand"));
			ToolPlacementOffset = FVector(0.0f, 0.0f, 50.0f);
			break;

		case EPlacementPreset::EPS_UVLightRack:
			AcceptedToolType = EToolType::ETT_UVLight;
			PlacementLabel = FText::FromString(TEXT("UV Light Rack"));
			ToolPlacementOffset = FVector(0.0f, 0.0f, 60.0f);
			break;

		case EPlacementPreset::EPS_GenericShelf:
			AcceptedToolType = EToolType::ETT_Any;
			PlacementLabel = FText::FromString(TEXT("Tool Shelf"));
			ToolPlacementOffset = FVector(0.0f, 0.0f, 40.0f);
			break;

		case EPlacementPreset::EPS_Custom:
		default:
			// No change, use current settings
			break;
	}
}

void ASimplePlacementPoint::SetupPlacementPoint(EToolType AcceptedType, FText Label, FVector Offset)
{
	AcceptedToolType = AcceptedType;
	PlacementLabel = Label;
	ToolPlacementOffset = Offset;
}

#if WITH_EDITOR
void ASimplePlacementPoint::DrawDebugPoint()
{
	if (GetWorld())
	{
		FVector PlacementLocation = GetToolPlacementLocation();
		DrawDebugSphere(GetWorld(), PlacementLocation, 15.0f, 8, FColor::Green, false, 0.0f, 0, 2.0f);
		DrawDebugDirectionalArrow(GetWorld(), GetActorLocation(), PlacementLocation, 20.0f, FColor::Cyan, false, 0.0f, 0, 1.0f);
	}
}

void ASimplePlacementPoint::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	DrawDebugPoint();
}
#endif
