// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InspectionProp.h"
#include "BaseAnomaly.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API ABaseAnomaly : public AInspectionProp
{
	GENERATED_BODY()
	
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Details")
	float MaxInspectionTime = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Details")
	float CurrentInspectionTime = 0.0f;

	virtual void OnInteract() override;

};
