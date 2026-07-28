// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "ST_DialogueLine.h"
#include "DialogueManagerSubsystem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueRequested, const FST_DialogueLine&, DialogueLine);

/**
 * Global Dialogue Manager Subsystem to handle dialogue requests from anywhere in the game.
 */
UCLASS()
class INLANDTOLL_API UDialogueManagerSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	// Event that the UI DialogueBox will listen to
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnDialogueRequested OnDialogueRequested;

	/**
	 * Requests a dialogue to be displayed.
	 * Can be called from C++ or Blueprints.
	 */
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void PlayDialogue(const FST_DialogueLine& DialogueLine)
	{
		OnDialogueRequested.Broadcast(DialogueLine);
	}

    /**
     * Helper to play multiple lines sequentially (basic implementation)
     */
    UFUNCTION(BlueprintCallable, Category = "Dialogue")
    void PlayDialogueSequence(const TArray<FST_DialogueLine>& DialogueLines)
    {
        for (const FST_DialogueLine& Line : DialogueLines)
        {
            // Note: A real sequence would need a timer/queue, 
            // but for now we broadcast them. UI can handle queuing if needed.
            PlayDialogue(Line);
        }
    }
};