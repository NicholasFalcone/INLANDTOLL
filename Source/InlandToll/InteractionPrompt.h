// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "InteractionPrompt.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API UInteractionPrompt : public UUserWidget
{
	GENERATED_BODY()
	
	private:
		UPROPERTY(meta = (BindWidget))
		class UTextBlock* PromptText;

	public:
		UFUNCTION(BlueprintCallable)
		void SetPromptText(const FText& NewText);
		UFUNCTION(BlueprintCallable)
		void Show(bool bShow);
};
