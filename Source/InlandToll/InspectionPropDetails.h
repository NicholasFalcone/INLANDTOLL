// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "InspectionPropDetails.generated.h"

USTRUCT(BlueprintType)
struct INLANDTOLL_API FInspectionPropDetails
{
    GENERATED_BODY()


    public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Prop Details")
    FString PropName;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Prop Details")
    FString PropDescription;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Prop Details")
    UTexture2D* PropImage;

    FInspectionPropDetails()
        : PropName(TEXT(""))
        , PropDescription(TEXT(""))
        , PropImage(nullptr)
    {}
};