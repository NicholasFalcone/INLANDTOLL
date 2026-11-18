// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Camera/CameraComponent.h"
#include "ToolPlacementManager.generated.h"

class AATool;
class AToolPlacementPoint;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class INLANDTOLL_API UToolPlacementManager : public UActorComponent
{
	GENERATED_BODY()

public:
	UToolPlacementManager();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// Distance to check for nearby placement points
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement")
	float InteractionRadius = 500.0f;

	// The currently equipped tool
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Placement")
	class AATool* EquippedTool = nullptr;

	// The closest highlighted placement point
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Placement")
	class AToolPlacementPoint* HighlightedPlacementPoint = nullptr;

	// Try to place the equipped tool at the highlighted placement point
	UFUNCTION(BlueprintCallable, Category = "Placement")
	bool TryPlaceCurrentTool();

	// Try to pick up a tool from the highlighted placement point
	UFUNCTION(BlueprintCallable, Category = "Placement")
	class AATool* TryPickupToolFromPlacement();

	// Update the highlighted placement point based on what the player is looking at
	UFUNCTION(BlueprintCallable, Category = "Placement")
	void UpdateHighlightedPlacementPoint();

	// Set the currently equipped tool
	UFUNCTION(BlueprintCallable, Category = "Placement")
	void SetEquippedTool(class AATool* NewTool);

	// Get the currently equipped tool
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Placement")
	class AATool* GetEquippedTool() const { return EquippedTool; }

	// Get all placement points in the world
	UFUNCTION(BlueprintCallable, Category = "Placement")
	void GetAllPlacementPoints(TArray<class AToolPlacementPoint*>& OutPlacementPoints);

private:
	// Previously highlighted placement point
	class AToolPlacementPoint* PreviousHighlightedPoint = nullptr;

	// Find the closest placement point within interaction radius
	class AToolPlacementPoint* FindClosestPlacementPoint();

	// Check line trace to see what the player is looking at
	class AToolPlacementPoint* GetLookAtPlacementPoint();
};
