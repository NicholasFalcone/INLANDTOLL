// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Tools/UVLight.h"
#include "BaseInteractable.h"
#include "HorrorCharacter.h"
#include "InteractableAnomaly.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractableAnomalySpottedByUVLight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractableAnomalyStoppedBeingSpottedByUVLight);
UCLASS()
class INLANDTOLL_API AInteractableAnomaly : public ABaseInteractable
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

	bool IsSpottedByUVLight();
	UPROPERTY()
	AHorrorCharacter* PlayerCharacter;
	UPROPERTY()
	AUVLight* UVLight;


public:
	virtual void Tick(float DeltaTime) override;

	/// --- Variable User of UV Light Interaction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tools Interaction")
	bool bIsBeingSpottedByUVLight = false;
	UPROPERTY(BlueprintAssignable, Category = "Tools Interaction")
	FOnInteractableAnomalyStoppedBeingSpottedByUVLight OnStoppedBeingSpottedByUVLight;
	UPROPERTY(BlueprintAssignable, Category = "Tools Interaction")
	FOnInteractableAnomalySpottedByUVLight OnSpottedByUVLight;
	void OnStartSpottedByUVLight();
	void OnStopBeingSpottedByUVLight();
	/// --- Variable User of Temperature Interaction
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Tools Interaction")
	float Temperature = 25.0f;
};
