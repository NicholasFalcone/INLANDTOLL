// Fill out your copyright notice in the Description page of Project Settings.

#include "InteractionPrompt.h"

void UInteractionPrompt::SetPromptText(const FText& NewText)
{
	if (PromptText)
	{
		PromptText->SetText(NewText);
	}
}

void UInteractionPrompt::Show(bool bShow)
{
	SetVisibility(bShow ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
