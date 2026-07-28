// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InspectionPropDetails.h"
#include "ComputerWidget.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API UComputerWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category = "Computer")
	void UpdateBannedList(const TArray<FInspectionPropDetails>& BannedProps);
};
