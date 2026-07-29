// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractable.h"
#include "BannedList.h"
#include "Components/WidgetComponent.h"
#include "ComputerWidget.h"
#include "Computer.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API AComputer : public ABaseInteractable
{
	GENERATED_BODY()
	
public:
	AComputer();

protected:
	void BeginPlay() override;
	void Tick(float DeltaTime) override;


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Data")
	TArray<UBannedList*> BannedLists;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* ComputerScreenWidget;

	UPROPERTY(BlueprintReadOnly, Category = "UI")
	UComputerWidget* ComputerScreenWidgetInstance = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Computer")
	void RefreshBannedListUI();

	UFUNCTION(BlueprintCallable, Category = "Computer")
	void BlackoutComputerScreen();

	UFUNCTION(BlueprintCallable, Category = "Computer")
	void PowerOnComputerScreen();

};
