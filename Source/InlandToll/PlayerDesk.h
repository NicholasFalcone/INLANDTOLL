// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InspectionDayDataAsset.h"
#include "GameFramework/Actor.h"
#include "PlayerDesk.generated.h"

class ATablet;
class ABaseInteractable;
class UWidgetComponent;
class ADesk;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSurvayCompleted);

UCLASS()
class INLANDTOLL_API APlayerDesk : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APlayerDesk();

public:
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactable")
    TObjectPtr<ABaseInteractable> PowerButtonComponent;

    // Child Actor Components per i pulsanti
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interactable")
    TObjectPtr<ABaseInteractable> PrintDetailsButtonComponent;

	    // Monitor UI
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UWidgetComponent> LeftMonitor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Questionary")
	FInspectionDayDataAsset DailyQuestionary;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnSurvayCompleted OnSurvayCompleted;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Table")
	ATablet* TabletInstance;

	ADesk* GeneralDeskInstance;

    const UAnomalyData* CurrentAnomalyDetails;
	
	ATablet* GetTabletInstance();
	void DeliverAnomalySheet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void OnPowerButtonPressed();
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void OnPrintDetailsButtonPressed();
	UFUNCTION(BlueprintCallable, Category = "Interactable")
	void OnDayshiftEnded();

	int32 GetCurrentDayReach() const;

};
