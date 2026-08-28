#pragma once

#include "CoreMinimal.h"
#include "Engine/UserDefinedEnum.h" // or other necessary core headers
#include "LoseReason.generated.h"


UENUM(BlueprintType)
enum class E_LOSE_REASON :uint8
{
    LOSE_REASON_ERRORS UMETA(DisplayName = "Too many errors"),
    LOSE_REASON_ANOMALY UMETA(DisplayName = "Anomaly"),
};