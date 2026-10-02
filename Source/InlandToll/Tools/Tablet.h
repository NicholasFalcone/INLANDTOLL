// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Anomaly/InspectableAnomaly.h"
#include "BaseInteractable.h"
#include "InspectionCarDataAsset.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "HorrorCharacter.h"
#include "UI/TableUI.h"
#include "Tablet.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API ATablet : public ABaseInteractable
{
	GENERATED_BODY()
	
protected:
	virtual void BeginPlay() override;

public:
	ATablet();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tablet")
	UWidgetComponent* TabletWidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tablet")
	TSubclassOf<UTableUI> TabletWidgetClass;

	TObjectPtr<UTableUI> TabletWidget;

	AHorrorCharacter* PlayerCharacter;

	void UpdateAnomaly(const FInspectionData& currentInspectionData);

	void RemoveSheet();

	bool bHasSheet = false;

	virtual void OnInteract() override;
	virtual void OnEndInteract() override;
public:
	bool CanBeDelivered() const;
	bool HasSheet() const;
};
