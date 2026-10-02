// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenu.generated.h"

class ADesk;

UCLASS()
class INLANDTOLL_API UInGameMenu : public UUserWidget
{
	GENERATED_BODY()

protected:
	void NativeConstruct() override;

private:
	TObjectPtr<class ADesk> DeskInstance;
	
public:
	UFUNCTION(BlueprintCallable, Category = "InGameMenu")
	void OpenPauseMenu();
	
	UFUNCTION(BlueprintCallable, Category = "InGameMenu")
	void ClosePauseMenu();

	UFUNCTION(BlueprintCallable, Category = "InGameMenu")
	void BackToGame();

	UFUNCTION(BlueprintCallable, Category = "InGameMenu")
	void BackToMainMenu();

	UFUNCTION(BlueprintCallable, Category = "InGameMenu")
	void RestartGame();

};
