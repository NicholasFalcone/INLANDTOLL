// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ATool.h"
#include "UVLight.generated.h"

class USpotLightComponent;

/**
 * 
 */
UCLASS()
class INLANDTOLL_API AUVLight : public AATool
{
	GENERATED_BODY()

public:
	AUVLight();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UV Light", meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* UVSpotLight;

public:
	virtual void OnUsed() override;
	virtual void OnEquipped(class ACharacter* InOwnerCharacter) override;
	virtual void OnUnequipped() override;
	
};
