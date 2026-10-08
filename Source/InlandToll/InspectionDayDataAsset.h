// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnomalyData.h"
#include "InspectionDayDataAsset.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct INLANDTOLL_API FInspectionDayDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Data")
	TArray<UAnomalyData*> InspectionData;
};
