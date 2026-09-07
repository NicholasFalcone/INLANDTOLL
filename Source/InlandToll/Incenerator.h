// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Crane.h"
#include "Incenerator.generated.h"


UCLASS()
class INLANDTOLL_API AIncenerator : public AActor
{
	GENERATED_BODY()
	
private:
	ACrane* CraneInstance;

public:	
	// Sets default values for this actor's properties
	AIncenerator();

protected:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection Car Manager")
	USplineComponent* SplinePath;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Incenerator")
	void StartInceneratorSequence();

	UFUNCTION(BlueprintImplementableEvent, Category = "Incenerator")
	void OnAnomalyDropped(AActor* Anomaly);

	void NotifyAnomalyDropped(AActor* Anomaly);

	void BurnAnomaly(AActor* Anomaly);

};
