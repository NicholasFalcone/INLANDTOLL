// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EPlayerState.h"
#include "InlandTollPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class AHorrorCharacter;

/**
 *  Simple first person Player Controller
 *  Manages the input mapping context.
 *  Overrides the Player Camera Manager class.
 */
UCLASS(abstract, config="Game")
class INLANDTOLL_API AInlandTollPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	/** Constructor */
	AInlandTollPlayerController();

	UFUNCTION(BlueprintCallable, Category = "Player State")
	bool RequestInteractionState(EPlayerState NewState, UUserWidget* WidgetToFocus = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Player State")
	void ResumeFromPause();

	UFUNCTION(BlueprintCallable, Category = "Player State")
	bool PopInteractionState(EPlayerState ExpectedState);

	UFUNCTION(BlueprintCallable, Category = "Player State")
	void EndInteractionState(EPlayerState State);

	UFUNCTION(BlueprintPure, Category = "Player State")
	EPlayerState GetInteractionState() const { return CurrentPlayerState; }

	UFUNCTION(BlueprintPure, Category = "Player State")
	bool HasInteractionState(EPlayerState State) const;

	// Forces the controller back to Unconstrained, also from Dead/Paused (used when restarting the game)
	UFUNCTION(BlueprintCallable, Category = "Player State")
	void ResetInteractionState();

protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Gameplay initialization */
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

private:
	void ApplyPlayerState(EPlayerState NewState, UUserWidget* WidgetToFocus);
	void ApplyInputBlock(bool bBlockMovement, bool bBlockLook);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Player State", meta = (AllowPrivateAccess = "true"))
	EPlayerState CurrentPlayerState = EPlayerState::Unconstrained;

	TArray<EPlayerState> PreviousInteractionStates;
	bool bMovementInputBlocked = false;
	bool bLookInputBlocked = false;
};
