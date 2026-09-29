// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenu.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API UInGameMenu : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "InGameMenu")
	void OpenPauseMenu();
	
	UFUNCTION(BlueprintCallable, Category = "InGameMenu")
	void ClosePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "InGameMenu")
	void BackToGame();

	UFUNCTION(BlueprintCallable, Category = "InGameMenu")
	void BackToMainMenu();

};
