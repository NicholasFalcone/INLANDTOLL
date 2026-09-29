// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueBox.h"
#include "DialogueManagerSubsystem.h"
#include "ST_DialogueLine.h"

void UDialogueBox::NativeConstruct()
{
	Super::NativeConstruct();

	// Bind to the dialogue manager subsystem
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UDialogueManagerSubsystem* DialogueSubsystem = GI->GetSubsystem<UDialogueManagerSubsystem>())
		{
			DialogueSubsystem->OnDialogueRequested.AddDynamic(this, &UDialogueBox::HandleDialogueRequested);
		}
	}
    HideDialogueLine(); // Ensure the dialogue box is hidden initially
}

void UDialogueBox::NativeDestruct()
{
	if (UGameInstance* GI = GetGameInstance())
	{
		if (UDialogueManagerSubsystem* DialogueSubsystem = GI->GetSubsystem<UDialogueManagerSubsystem>())
		{
			DialogueSubsystem->OnDialogueRequested.RemoveAll(this);
		}
	}

	Super::NativeDestruct();
}

void UDialogueBox::HandleDialogueRequested(const FST_DialogueLine& DialogueLine)
{
	// Forward the request to the Blueprint event for visual display
	ShowDialogueLine(DialogueLine.SpeakerName, DialogueLine.DialogueText, DialogueLine.DisplayDuration);
}

