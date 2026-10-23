// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InlandTollPlayerController.h"
#include "HorrorPlayerController.generated.h"

class UUI_Master;
class UDialogueBox;

/**
 *  Player Controller for a first person horror game
 *  Manages input mappings
 *  Manages UI
 */
UCLASS(abstract, config="Game")
class INLANDTOLL_API AHorrorPlayerController : public AInlandTollPlayerController
{
	GENERATED_BODY()
	
protected:

	/** Type of UI widget to spawn */
	UPROPERTY(EditAnywhere, Category="Horror|UI")
	TSubclassOf<UUI_Master> MainUIClass;

	/** Pointer to the UI widget */
	UPROPERTY()
	TObjectPtr<UUI_Master> MainUI;
	
	/** Type of UI widget to spawn */
	UPROPERTY(EditAnywhere, Category="Horror|UI")
	TSubclassOf<UDialogueBox> DialogueBoxWidgetClass;
	UPROPERTY()
	TObjectPtr<UDialogueBox> DialogueBoxWidget;

public:

	/** Constructor */
	AHorrorPlayerController();

protected:
	/** Possessed pawn initialization */
	virtual void OnPossess(APawn* InPawn) override;

	virtual void OnUnPossess() override;
};
