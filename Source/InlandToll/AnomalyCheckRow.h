
#pragma once

#include "CoreMinimal.h"
#include "AnomalyCheckRow.generated.h"

USTRUCT(BlueprintType)
struct INLANDTOLL_API FAnomalyCheckRow
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Check Row")
    FString rowSentence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Check Row")
    FString Answer1;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Check Row")
    FString Answer2;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Check Row")
    FString Answer3;

    FAnomalyCheckRow()
        : rowSentence(TEXT(""))
        , Answer1(TEXT(""))
        , Answer2(TEXT(""))
        , Answer3(TEXT(""))
    {}

};