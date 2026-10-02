// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TableUI.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSheetRemoved);

UCLASS()
class INLANDTOLL_API UTableUI : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "TableUI")
	FOnSheetRemoved OnSheetRemoved;

public:
	void UpdateAnomaly(const FInspectionData& currentInspectionData);

	UFUNCTION(BlueprintImplementableEvent, Category = "Custom Events")
	void CallUpdateAnomaly(const FInspectionData& currentInspectionData);
	
	void RemoveSheet();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "TableUI")
	bool bSheetCompiled = false;
	
};
