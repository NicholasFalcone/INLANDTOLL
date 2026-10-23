// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "Camera/CameraComponent.h"
#include "Anomaly/InteractableAnomaly.h"
#include "InterdimentionalPortal.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API AInterdimentionalPortal : public AInteractableAnomaly
{
	GENERATED_BODY()

public:
	AInterdimentionalPortal();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	bool isUnlocked;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	FString UnlockCode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	UWidgetComponent* LockerCode;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Portal")
	UCameraComponent* PortalCamera;

private:
	bool bExitInputBound = false;

public:
	UFUNCTION(BlueprintCallable, Category="Portal")
	void SetPlayerCameraToLocker();

	UFUNCTION(BlueprintCallable, Category="Portal")
	void CheckUnlockCode(FString EnteredCode);

	UFUNCTION(BlueprintCallable, Category="Portal")
	void UnlockPortal();

	UFUNCTION(BlueprintCallable, Category="Portal")
	void ExitPlayerCameraFromLocker();

};
