// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Variant_Horror/HorrorCharacter.h"
#include "InspectionManager.h"
#include "UI_Master.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API UUI_Master : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void SetupCharacter(AHorrorCharacter* HorrorCharacter);
	void SetupManager(AInspectionManager* CarManager);

	UFUNCTION()
	void HandleErrorCountChanged(int32 NewErrorCount);

	UFUNCTION()
	void HandleGameOver();

	/** Passes control to Blueprint to update the sprint meter status */
	UFUNCTION(BlueprintImplementableEvent, Category="Events", meta = (DisplayName = "User error count changed"))
	void BP_UserErrorCountChanged(int32 NewErrorCount);

	UFUNCTION(BlueprintImplementableEvent, Category="Events", meta = (DisplayName = "Game Over"))
	void BP_OnGameOver();
};
