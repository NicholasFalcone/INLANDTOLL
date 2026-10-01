// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InspectionData.h"
#include "Components/SceneComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/StaticMesh.h"
#include "InspectionCarDataAsset.h"
#include "InspectionManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDailyInspectionLimitReached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnErrorCountChanged, int32, NewErrorCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMaxErrorsReached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDailyInspectionCountChanged, int32, NewDailyInspectionCount);

class ADesk;
class AHorrorCharacter;
class ATablet;
class AInspectionPayload;

UCLASS()
class INLANDTOLL_API AInspectionManager : public AActor
{
	GENERATED_BODY()
	
private:
	ATablet* TabletInstance;

public:	
	// Sets default values for this actor's properties
	AInspectionManager();
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	TArray<UStaticMesh*> CarMeshes;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	TArray<UInspectionCarDataAsset*> InspectionDataArray;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	int32 CurrentInspectionIndex = 0;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection Car Manager")
	USplineComponent* SplinePath;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	TSubclassOf<AInspectionPayload> CarTemplate;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection Car Manager")
	AInspectionPayload* CurrentInspectionCar;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	int32 MaxErrorsAllowed = 3;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection Car Manager")
	int32 CurrentErrors = 0;
	
	UPROPERTY(BlueprintAssignable, Category = "Inspection Car Manager")
	FOnErrorCountChanged OnErrorCountChanged;

	UPROPERTY(BlueprintAssignable, Category = "Inspection Car Manager")
	FOnMaxErrorsReached OnMaxErrorsReached;

	UPROPERTY(BlueprintAssignable, Category = "Inspection Car Manager")
	FOnDailyInspectionLimitReached OnDailyInspectionLimitReached;
	
	UPROPERTY(BlueprintAssignable, Category = "Inspection Car Manager")
	FOnDailyInspectionCountChanged OnDailyInspectionCountChanged;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	TArray<FST_DialogueLine> ErrorDialogueLines;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection Car Manager")
	int32 CurrentDayInspection = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	int32 InspectionToDailyReach = 5;

	AHorrorCharacter* PlayerCharacter;
	ADesk* DeskInstance;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Exec, Category = "Inspection Car Manager")
	void SpawnNextAnomaly();
	UFUNCTION(BlueprintCallable, Exec, Category = "Inspection Car Manager")
	void PassCurrentInspectionDataToAnomaly();
	UFUNCTION(BlueprintCallable, Exec, Category = "Inspection Car Manager")
	void RejectCurrentInspectedAnomaly();

	UFUNCTION()
	void HandleCarReachedEnd(AInspectionPayload* Car);
	
	UFUNCTION(BlueprintCallable, Exec, Category = "Inspection Car Manager")
	void RestartGame();

	UFUNCTION(BlueprintCallable, Exec, Category = "Inspection Car Manager")
	void DestroyCurrentInspection();
	UFUNCTION(BlueprintCallable, Exec, Category = "Inspection Car Manager")
	void HandlePlayerDied();
	UFUNCTION(BlueprintCallable, Exec, Category = "Inspection Car Manager")
	void DestroyAllAnomaly();

};
