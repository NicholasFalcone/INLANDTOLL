// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InspectionProp.h"
#include "ST_DialogueLine.h"
#include "AnomalyCheckRow.h"
#include "InspectionData.generated.h"

USTRUCT(BlueprintType)
struct INLANDTOLL_API FInspectionData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Data")
	FString InspectionName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Data")
	FString InspectionDescription;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Data")
	int32 InspectionID;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Data")
	FString AttachedSocketName;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Data")
	TSubclassOf<AInspectionProp> InspectionPropClass;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue Lines")
	TArray<FST_DialogueLine> InspectionDialogueLines;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Details")
	TArray<FAnomalyCheckRow> AnomalyCheckRows;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Details")
	bool bIsDangerous = false;

public:
	FInspectionData();
	~FInspectionData();
};
