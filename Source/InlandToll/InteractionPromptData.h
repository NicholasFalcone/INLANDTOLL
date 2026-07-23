#pragma once

#include "CoreMinimal.h"
#include "InteractionPromptData.generated.h"

USTRUCT(BlueprintType)
struct INLANDTOLL_API FInteractionPromptData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	bool bShowInteractionPrompt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	FVector InteractionPromptOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Data")
	FText InteractionPrompt;

	FInteractionPromptData()
	{
		bShowInteractionPrompt = true;
		InteractionPromptOffset = FVector::ZeroVector;
		InteractionPrompt = FText::FromString("Press E to interact");
	}
};
