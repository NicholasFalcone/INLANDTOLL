// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/SkeletalMesh.h"
#include "InspectionData.h"
#include "InspectionCarDataAsset.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API UInspectionCarDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Data")
	USkeletalMesh* CarMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Data")
	FInspectionData InspectionData;
};
