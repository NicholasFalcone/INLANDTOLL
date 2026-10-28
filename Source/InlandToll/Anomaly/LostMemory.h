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
	// One photo per state: state N shows PostcardTextures[N - 1]
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Postcard")
	TArray<TObjectPtr<UTexture>> PostcardTextures;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Postcard")
	FName TextureParameterName = TEXT("BaseColor");

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Postcard")
	int32 MaterialSlotIndex = 0;

	// Speed of the automatic turn once the postcard is locked
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Postcard")
	float AutoRotateSpeed = 4.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Postcard")
	int32 CurrentState = 1;

protected:
	virtual void BeginPlay() override;
	virtual void OnInteract() override;

	void ApplyCurrentTexture();
	bool IsBackFacingPlayer() const;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> PostcardMaterial;

	bool bShowingBack = false;
	bool bLocked = false;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void OnRotate(float delta) override;

};
