// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAnomaly.h"
#include "Components/AudioComponent.h"
#include "AnomalyDoll.generated.h"

UENUM(BlueprintType)
enum class EShakeRotationAxis : uint8
{
	Forward,
	Right,
	Up
};

/**
 * 
 */
UCLASS()
class INLANDTOLL_API AAnomalyDoll : public ABaseAnomaly
{
	GENERATED_BODY()
	
	public:
		AAnomalyDoll();

	protected:
		/** Quale asse dell'Actor monitorare per le rotazioni (es. UpVector per scuotimenti in torsione/rollio) */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake Detection|Settings")
		EShakeRotationAxis AxisToTrack = EShakeRotationAxis::Up;
	/** Velocità angolare minima per considerare il movimento valido (gradi/secondo) */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake Detection|Settings", meta = (ClampMin = "0.0"))
		float MinAngularSpeed = 180.0f; // es. 180 deg/s

		/** Soglia del Dot Product per l'inversione di rotazione (-1.0 = inversione totale) */
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake Detection|Settings", meta = (ClampMin = "-1.0", ClampMax = "0.0"))
		float InversionThreshold = -0.3f;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake Detection|Settings", meta = (ClampMin = "0.0"))
		float ShakeAccumulationRate = 3.0f;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake Detection|Settings", meta = (ClampMin = "0.0"))
		float DecayRate = 4.0f;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake Detection|Settings", meta = (ClampMin = "0.1"))
		float ShakeThreshold = 1.0f;

		UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Shake")
		UAudioComponent* AudioComponent;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Shake")
		USoundBase* ShakeSound;

	private:
		FVector LastTrackedAxis = FVector::ZeroVector;
		FVector LastAngularVelocity = FVector::ZeroVector;

		UPROPERTY(VisibleInstanceOnly, Transient, Category = "Shake Detection|Debug")
		float ShakeScore = 0.0f;

		UPROPERTY(VisibleInstanceOnly, Transient, Category = "Shake Detection|Debug")
		bool bHasBeenShaken = false;

		FVector GetTargetAxisVector(const AActor* TargetActor) const;

	public:
		virtual void BeginPlay() override;
		virtual void Tick(float DeltaTime) override;
		virtual void Destroyed() override;
		virtual void OnEndInteract() override;

	public:
		void OnShaken();
		void OnTabletTicked();

	protected:
		void CheckBeenShaken(float DeltaTime);
};
