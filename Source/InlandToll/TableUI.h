// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TableUI.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API UTableUI : public UUserWidget
{
	GENERATED_BODY()

public:
	void UpdateAnomaly(const FInspectionData& currentInspectionData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Custom Events")
	void CallUpdateAnomaly(const FInspectionData& currentInspectionData);
};
