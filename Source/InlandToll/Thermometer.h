// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ATool.h"
#include "Thermometer.generated.h"

class UTextRenderComponent;

/**
 * 
 */
UCLASS()
class INLANDTOLL_API AThermometer : public AATool
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AThermometer();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Thermometer", meta = (AllowPrivateAccess = "true"))
	UTextRenderComponent* TemperatureTextComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thermometer")
	float AmbientTemperature;

	UPROPERTY(BlueprintReadWrite, Category = "Thermometer")
	float TargetTemperature;

	UPROPERTY(BlueprintReadOnly, Category = "Thermometer")
	float CurrentTemperature;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thermometer")
	float InterpSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Thermometer")
	float TraceDistance;

	void UpdateTargetTemperature();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	virtual void OnUsed() override;
	virtual void OnEquipped(class ACharacter* InOwnerCharacter) override;
	virtual void OnUnequipped() override;
};
