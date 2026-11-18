// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ToolPlacementPoint.h"
#include "SimplePlacementPoint.generated.h"

// Enum per i preset disponibili
UENUM(BlueprintType)
enum class EPlacementPreset : uint8
{
	EPS_Custom UMETA(DisplayName = "Custom"),
	EPS_ThermometerStand UMETA(DisplayName = "Thermometer Stand"),
	EPS_UVLightRack UMETA(DisplayName = "UV Light Rack"),
	EPS_GenericShelf UMETA(DisplayName = "Generic Shelf"),
};

/**
 * Versione semplificata di AToolPlacementPoint
 * Fornisce templates predefiniti per diversi tipi di placement points
 */
UCLASS()
class INLANDTOLL_API ASimplePlacementPoint : public AToolPlacementPoint
{
	GENERATED_BODY()

public:
	ASimplePlacementPoint();



	// Applica un preset predefinito
	UFUNCTION(BlueprintCallable, Category = "Placement|Setup")
	void ApplyPreset(EPlacementPreset Preset);

	// Setup semplificato
	UFUNCTION(BlueprintCallable, Category = "Placement|Setup")
	void SetupPlacementPoint(EToolType AcceptedType, FText Label, FVector Offset = FVector::ZeroVector);

#if WITH_EDITOR
	// Visualizza l'offset nel viewport dell'editor
	virtual void DrawDebugPoint();
	virtual void OnConstruction(const FTransform& Transform) override;
#endif
};
