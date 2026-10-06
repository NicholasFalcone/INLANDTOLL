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

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInspectionError);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInspectionEnded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAnomalyDetailsChange, const FInspectionData&, NewAnomalyDetails);

class ADesk;
class AHorrorCharacter;
class AInspectionPayload;

UCLASS()
class INLANDTOLL_API AInspectionManager : public AActor
{
	GENERATED_BODY()
	
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
	AInspectionPayload* CurrentInspectionPayload;
	int32 CurrentErrors = 0;

	UPROPERTY(BlueprintAssignable, Category = "Inspection Car Manager")
	FOnInspectionError OnInspectionError;
	UPROPERTY(BlueprintAssignable, Category = "Inspection Car Manager")
	FOnInspectionEnded OnInspectionEnded;
	UPROPERTY(BlueprintAssignable, Category = "Inspection Car Manager")
	FOnAnomalyDetailsChange OnAnomalyDetailsChange;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	TArray<FST_DialogueLine> ErrorDialogueLines;

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
