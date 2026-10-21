// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractable.h"
#include "Camera/CameraComponent.h"
#include "Bed.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerSleeping);

UCLASS()
class INLANDTOLL_API ABed : public ABaseInteractable
{
	GENERATED_BODY()
	

public:
	// Sets default values for this actor's properties
	ABed();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	UPROPERTY(BlueprintAssignable, Category = "Bed")
	FOnPlayerSleeping OnPlayerSleeping;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnInteract() override;
	UFUNCTION()
	void HandleDailyInspectionLimitReached();
};
