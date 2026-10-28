// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Anomaly/InspectableAnomaly.h"
#include "ST_DialogueLine.h"
#include "Flower.generated.h"


UCLASS()
class INLANDTOLL_API AFlower : public AInspectableAnomaly
{
	GENERATED_BODY()

private:
	float CurrentTime = 0;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FST_DialogueLine> OnUpsideDownLine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FST_DialogueLine> OnCheckBoxChangeLine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FST_DialogueLine> OnDangerDeclarationLine;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flower")
	float PetalLossRateTime;
	TArray<UStaticMeshComponent*> Petals;

	// Found at BeginPlay via the "Head" component tag
	UPROPERTY(BlueprintReadOnly, Category = "Flower")
	TObjectPtr<UStaticMeshComponent> Head;

	// Offset applied after the look-at, to match the head mesh's facing axis
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Flower")
	FRotator HeadLookAtOffset = FRotator::ZeroRotator;

protected:
	virtual void BeginPlay() override;

public:
	AFlower();
	virtual void OnRotate(float delta) override;
	virtual void Tick(float DeltaTime) override;

public:
	int32 GetPetalCount() const { return Petals.Num(); }
	void LosePetal();
	bool HasPetals();
	void UpdateHeadLookAt();
	void DestroyPetal(UStaticMeshComponent* Petal);
};
