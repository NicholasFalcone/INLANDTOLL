// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InspectionCar.h"
#include "InspectionData.h"
#include "Components/SceneComponent.h"
#include "InspectionCarDataAsset.h"
#include "InspectionCarManager.generated.h"

UCLASS()
class INLANDTOLL_API AInspectionCarManager : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInspectionCarManager();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	AInspectionCar* InspectionCars;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	TArray<UInspectionCarDataAsset*> InspectionDataArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	int32 CurrentInspectionIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection Car Manager")
	USceneComponent* StartingPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection Car Manager")
	USceneComponent* EndingPoint;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Exec, Category = "Inspection Car Manager")
	void SpawnNextInspectionCar();

};
