// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnomalyData.h"
#include "TableUI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSheetRemoved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSheetEnabled);

UCLASS()
class INLANDTOLL_API UTableUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "TableUI")
	FOnSheetRemoved OnSheetRemoved;
	
	UPROPERTY(BlueprintAssignable, Category = "TableUI")
	FOnSheetEnabled OnSheetEnabled;

public:
	void UpdateAnomaly(const UAnomalyData* currentInspectionData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Custom Events")
	void CallUpdateAnomaly(const UAnomalyData* currentInspectionData);
	
	void RemoveSheet();

	void EnableSheet();
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TableUI")
	bool bSheetCompiled = false;
	
};
