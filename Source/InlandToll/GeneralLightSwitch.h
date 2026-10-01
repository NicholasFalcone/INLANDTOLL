// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/Light.h"
#include "BaseInteractable.h"
#include "GeneralLightSwitch.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLightsToggled, bool, bIsLightOn);
UCLASS()
class INLANDTOLL_API AGeneralLightSwitch : public ABaseInteractable
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lights")
	bool bIsLightOn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Lights")
	TArray<ALight*> Lights;

public:

	virtual void OnInteract() override;

	UFUNCTION(BlueprintCallable, Category="Lights")
	void ToggleLights();
	UFUNCTION(BlueprintCallable, Category="Lights")
	void SetLight(bool bNewState);
	UPROPERTY(BlueprintAssignable, Category="Lights")
	FOnLightsToggled OnLightsToggled;
};
