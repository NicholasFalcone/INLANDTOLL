
#pragma once

#include "CoreMinimal.h"
#include "AnomalyCheckRow.generated.h"

USTRUCT(BlueprintType)
struct INLANDTOLL_API FAnomalyCheckRow
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Check Row")
    FText rowSentence;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Check Row")
    FText Answer1;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Check Row")
    FText Answer2;
    
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Check Row")
    FText Answer3;

    FAnomalyCheckRow()
        : rowSentence(FText::GetEmpty())
        , Answer1(FText::GetEmpty())
        , Answer2(FText::GetEmpty())
        , Answer3(FText::GetEmpty())
    {}

};