// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ST_DialogueLine.generated.h"

USTRUCT(BlueprintType)
struct INLANDTOLL_API FST_DialogueLine
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString SpeakerName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	FString DialogueText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	float DisplayDuration;

	FST_DialogueLine()
		: SpeakerName(TEXT(""))
		, DialogueText(TEXT(""))
		, DisplayDuration(3.0f)
	{}
};
