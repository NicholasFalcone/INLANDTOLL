// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Anomaly/InteractableAnomaly.h"
#include "CorpseInSuite.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMaxTimeReached);

UCLASS()
class INLANDTOLL_API ACorpseInSuite : public AInteractableAnomaly
{
	GENERATED_BODY()


public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Corpse In Suite")
	float MaxTimeOpen = 10.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Corpse In Suite")
	float CurrentTimeOpen = 0.0f;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Corpse In Suite")
	bool isOpen = false;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnMaxTimeReached OnMaxTimeReachedEvent;

public:
	virtual void Tick(float DeltaTime) override;
	UFUNCTION(BlueprintCallable, Category = "Corpse In Suite")
	void SetSuitOpen(bool bOpen);
	UFUNCTION(BlueprintCallable, Category = "Corpse In Suite")
	void OnMaxTimeReached();

};
