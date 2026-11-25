// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseInteractable.h"
#include "InteractionComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INLANDTOLL_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

	// Radius around the camera; zero disables interaction.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction", meta = (ClampMin = "0.0", UIMin = "0.0", Units = "cm"))
	float InteractionDistance = 200.0f;

	UPROPERTY(Transient)
	TObjectPtr<ABaseInteractable> CurrentInteractable;

private:
	ABaseInteractable* CheckForInteractables();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	bool HasInteractableInRange() const
	{
		return IsValid(CurrentInteractable) && CurrentInteractable->bIsInteractable;
	}
};
