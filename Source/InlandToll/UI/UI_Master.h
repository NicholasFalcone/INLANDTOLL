// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Variant_Horror/HorrorCharacter.h"
#include "LoseReason.h"
#include "UI_Master.generated.h"

/**
 * 
 */
class ADesk;
class AHorrorCharacter;

UCLASS()
class INLANDTOLL_API UUI_Master : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetupCharacter(AHorrorCharacter* HorrorCharacter);
	void SetupManager(ADesk* DeskInstance);

	UFUNCTION()
	void HandleErrorCountChanged(int32 NewErrorCount);

	UFUNCTION()
	void HandleMaxErrorsReached();

	UFUNCTION()
	void HandlePlayerGettingCut();

	UFUNCTION()
	void HandleDailyInspectionLimitReached();

	UFUNCTION()
	void HandleDailyInspectionCountChanged(int32 NewDailyInspectionCount);

	UFUNCTION(BlueprintImplementableEvent, Category="Events", meta = (DisplayName = "Player Getting Cut"))
	void BP_OnPlayerGettingCut();

	UFUNCTION()
	void HandleGameOver();

	UFUNCTION()
	void HandleLoadNextDay();

	UFUNCTION(BlueprintImplementableEvent, Category="Events", meta = (DisplayName = "Load Next Day"))
	void BP_OnLoadNextDay();

	/** Passes control to Blueprint to update the sprint meter status */
	UFUNCTION(BlueprintImplementableEvent, Category="Events", meta = (DisplayName = "User error count changed"))
	void BP_UserErrorCountChanged(int32 NewErrorCount);

	UFUNCTION(BlueprintImplementableEvent, Category="Events", meta = (DisplayName = "Game Over"))
	void BP_OnGameOver(E_LOSE_REASON Reason);

	UFUNCTION(BlueprintImplementableEvent, Category="Events", meta = (DisplayName = "Daily Inspection Limit Reached"))
	void BP_OnDailyInspectionLimitReached();

	UFUNCTION(BlueprintImplementableEvent, Category="Events", meta = (DisplayName = "Daily Inspection Count Changed"))
	void BP_OnDailyInspectionCountChanged(int32 NewDailyInspectionCount);
};
