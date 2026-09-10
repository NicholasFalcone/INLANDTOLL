// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAnomaly.h"
#include "Components/WidgetComponent.h"
#include "BoxAnomaly.generated.h"

/**
 * 
 */
 // 1. Dichiarazione delle firme dei delegate (fuori dalla classe)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateAddressWidget, const FString&, NewAddress); 
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUpdateCodeWidget, const FString&, NewCode);

UCLASS()
class INLANDTOLL_API ABoxAnomaly : public ABaseAnomaly
{
	GENERATED_BODY()

public:
	ABoxAnomaly();

protected:
	virtual void BeginPlay() override;
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Box Details")
	FString Address = "123 Main St, Anytown, USA";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Box Details")
	FString Code = "ABC123";

	UPROPERTY(EditAnywhere, Category = "Box Details")
	TSubclassOf<UInteractionPrompt> BoxDetailsWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prompt Data")
	UWidgetComponent* AddressWidget;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prompt Data")
	UWidgetComponent* CodeWidget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Box Details")
	USoundBase* UpdateSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Box Details")
	float SoundCooldown = 3.0f;
	float LastSoundTime = -INFINITY;

public:
	virtual void OnInteract() override;

	void TriggerEventSound();

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnUpdateAddressWidget UpdateAddressWidget;
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnUpdateCodeWidget UpdateCodeWidget;

};
