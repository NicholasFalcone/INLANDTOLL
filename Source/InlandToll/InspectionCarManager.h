// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InspectionCar.h"
#include "InspectionData.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/StaticMesh.h"
#include "Tablet.h"
#include "InspectionCarDataAsset.h"
#include "InspectionCarManager.generated.h"

UCLASS()
class INLANDTOLL_API AInspectionCarManager : public AActor
{
	GENERATED_BODY()
	
private:
	ATablet* TabletInstance;

public:	
	// Sets default values for this actor's properties
	AInspectionCarManager();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	TArray<UStaticMesh*> CarMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	TArray<UInspectionCarDataAsset*> InspectionDataArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	int32 CurrentInspectionIndex = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection Car Manager")
	USplineComponent* SplinePath;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	TSubclassOf<AInspectionCar> CarTemplate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection Car Manager")
	AInspectionCar* CurrentInspectionCar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	int32 MaxErrorsAllowed = 3;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection Car Manager")
	int32 CurrentErrors = 0;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Exec, Category = "Inspection Car Manager")
	void SpawnNextInspectionCar();
	UFUNCTION(BlueprintCallable, Exec, Category = "Inspection Car Manager")
	void PassCurrentInspectionDataToCar();
	UFUNCTION(BlueprintCallable, Exec, Category = "Inspection Car Manager")
	void RejectCurrentInspectionCar();
	
protected:
	UFUNCTION()
	void HandleCarReachedEnd(AInspectionCar* Car);


};
