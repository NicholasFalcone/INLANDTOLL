// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ToolPlacementPoint.generated.h"

UENUM(BlueprintType)
enum class EToolType : uint8
{
	ETT_None = 0 UMETA(DisplayName = "None"),
	ETT_Thermometer = 1 UMETA(DisplayName = "Thermometer"),
	ETT_UVLight = 2 UMETA(DisplayName = "UV Light"),
	ETT_Tablet = 3 UMETA(DisplayName = "Tablet"),
	ETT_Any = 255 UMETA(DisplayName = "Any Tool")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnToolPlaced, class AATool*, PlacedTool, class AToolPlacementPoint*, PlacementPoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnToolRemoved, class AATool*, RemovedTool, class AToolPlacementPoint*, PlacementPoint);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlacementPointHighlighted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlacementPointUnhighlighted);

class AATool;
class ATablet;
class AThermometer;
class AUVLight;

UCLASS()
class INLANDTOLL_API AToolPlacementPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	AToolPlacementPoint();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USceneComponent* RootSceneComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* VisualizationMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UBoxComponent* InteractionVolume;

public:
	// Tool type this placement point accepts
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement")
	EToolType AcceptedToolType = EToolType::ETT_Any;

	// Label for this placement point (e.g., "Desk Left", "Cabinet Top")
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement")
	FText PlacementLabel = FText::FromString(TEXT("Tool Stand"));

	// Whether this placement point accepts tools
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement")
	bool bIsActive = true;

	// Whether a tool is currently placed here
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Placement")
	bool bHasToolPlaced = false;

	// The tool currently placed here
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Placement")
	class AATool* PlacedTool;

	// Offset from the placement point where the tool will be positioned
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement")
	FVector ToolPlacementOffset = FVector(0.0f, 0.0f, 0.0f);

	// Rotation for the placed tool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement")
	FRotator ToolPlacementRotation = FRotator::ZeroRotator;

	// If true, the tool will be highlighted when player looks at this point
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Placement")
	bool bHighlightPlacedTool = true;

	// Material to use when highlighted
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	class UMaterialInterface* HighlightMaterial;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnToolPlaced OnToolPlaced;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnToolRemoved OnToolRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlacementPointHighlighted OnPlacementPointHighlighted;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlacementPointUnhighlighted OnPlacementPointUnhighlighted;

public:	
	virtual void Tick(float DeltaTime) override;

	// Try to place a tool at this point
	UFUNCTION(BlueprintCallable, Category = "Placement")
	bool TryPlaceTool(class AATool* ToolToPlace);

	// Remove tool from this placement point
	UFUNCTION(BlueprintCallable, Category = "Placement")
	class AATool* RemoveTool();

	// Check if a tool type is accepted by this placement point
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Placement")
	bool CanAcceptToolType(EToolType ToolType) const;

	// Highlight the placement point
	UFUNCTION(BlueprintCallable, Category = "Visual")
	void Highlight();

	// Unhighlight the placement point
	UFUNCTION(BlueprintCallable, Category = "Visual")
	void Unhighlight();

	// Called when player interacts with the placement point (optional override)
	virtual void OnInteractionStarted() {};

	// Get the world position where the tool should be placed
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Placement")
	FVector GetToolPlacementLocation() const;

	// Get the rotation the tool should have
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Placement")
	FRotator GetToolPlacementRotation() const;

	// Get accepted tool type
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Placement")
	EToolType GetAcceptedToolType() const { return AcceptedToolType; }

	// Get placement label
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Placement")
	FText GetPlacementLabel() const { return PlacementLabel; }

private:
	// Called when the placed tool is re-equipped by the player
	UFUNCTION()
	void OnPlacedToolEquipped();
};
