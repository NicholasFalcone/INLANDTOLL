// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueBox.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API UDialogueBox : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	UFUNCTION()
	void HandleDialogueRequested(const FST_DialogueLine& DialogueLine);

public:
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void ShowDialogueLine(const FString& SpeakerName, const FString& DialogueText, float DisplayDuration);

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void HideDialogueLine();

};
