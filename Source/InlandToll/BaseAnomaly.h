// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InspectionProp.h"
#include "UVLight.h"
#include "AnomalyCheckRow.h"
#include "BaseAnomaly.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSpottedByUVLight);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStoppedBeingSpottedByUVLight);
UCLASS()
class INLANDTOLL_API ABaseAnomaly : public AInspectionProp
{
	GENERATED_BODY()
	
protected:
	
	UPROPERTY()
	AUVLight* UVLight;
	

protected:		
	bool IsSpottedByUVLight();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tools Interaction")
	bool bIsBeingSpottedByUVLight = false;
	UPROPERTY(BlueprintAssignable, Category = "Tools Interaction")
	FOnStoppedBeingSpottedByUVLight OnStoppedBeingSpottedByUVLight;
	UPROPERTY(BlueprintAssignable, Category = "Tools Interaction")
	FOnSpottedByUVLight OnSpottedByUVLight;
	void OnStartSpottedByUVLight();
	void OnStopBeingSpottedByUVLight();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Details")
	float MaxInspectionTime = -1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anomaly Details")
	float CurrentInspectionTime = 0.0f;

	bool bIsBeingInspected = false;

	virtual void Tick(float DeltaTime) override;

	virtual void OnInteract() override;
	virtual void OnEndInteract() override;
	virtual void OnInspectionProgress(float DeltaTime);
	virtual void OnRotate(float delta);

};
