// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAnomaly.h"
#include "BaseInteractable.h"
#include "InspectionCarDataAsset.h"
#include "Components/WidgetComponent.h"
#include "Blueprint/UserWidget.h"
#include "TableUI.h"
#include "Tablet.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API ATablet : public ABaseInteractable
{
	GENERATED_BODY()
	
public:
	ATablet();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tablet")
	UWidgetComponent* TabletWidgetComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tablet")
	TSubclassOf<UTableUI> TabletWidgetClass;

	void UpdateAnomaly(const FInspectionData& currentInspectionData);
};
