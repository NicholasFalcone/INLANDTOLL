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

	/** Introduction UI widget class */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horror|UI")
	TSubclassOf<UUserWidget> GameIntroductionWidgetClass;
	
	/** Pointer to the introduction widget */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Horror|UI")
	TObjectPtr<UUserWidget> GameIntroductionWidgetInstance;

public:

	/** Constructor */
	AHorrorPlayerController();

	/** Call this when the introduction is complete to exit the MainMenu state */
	UFUNCTION(BlueprintCallable, Category = "Horror|UI")
	void OnIntroductionComplete();

protected:
	/** Gameplay initialization */
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

private:
	/** Track if introduction has been shown */
	bool bGameIntroductionShown = false;
};
