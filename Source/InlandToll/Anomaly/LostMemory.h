// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Anomaly/InspectableAnomaly.h"
#include "LostMemory.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API ALostMemory : public AInspectableAnomaly
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float MonsterDistance = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float MaxMonsterDistance = 100;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Monster")
	float MaxAngleTolerance = 45;


protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void OnRotate(float delta) override;

};
