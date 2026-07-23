// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BaseInteractable.h"
#include "InteractionComponent.generated.h"

class AHorrorCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class INLANDTOLL_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UInteractionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionDistance = 200.0f;

	AHorrorCharacter* OwnerCharacter;
	ABaseInteractable* CurrentInteractable;

private:
	ABaseInteractable* CheckForInteractables();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void Init(AHorrorCharacter* InOwnerCharacter);

	bool HasInteractableInRange() const
	{
		return CurrentInteractable != nullptr;
	}
};
