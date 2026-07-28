// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InspectionProp.h"
#include "BannedList.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API UBannedList : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Banned Items")
	TArray<TSubclassOf<AInspectionProp>> BannedItems;
};
