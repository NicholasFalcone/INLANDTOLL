// Fill out your copyright notice in the Description page of Project Settings.


#include "AnomalyDoll.h"

AAnomalyDoll::AAnomalyDoll()
{
}

void AAnomalyDoll::BeginPlay()
{
	Super::BeginPlay();
	bHasBeenShaken = false;
	ShakeScore = 0.0f;
}

void AAnomalyDoll::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    if(bIsBeingInspected){
        CheckBeenShaken(DeltaTime);
    }
}

void AAnomalyDoll::Destroyed()
{
	Super::Destroyed();
}

FVector AAnomalyDoll::GetTargetAxisVector(const AActor* TargetActor) const
{
	if (!TargetActor) return FVector::ZeroVector;

	switch (AxisToTrack)
	{
		case EShakeRotationAxis::Forward:
			return TargetActor->GetActorForwardVector();
		case EShakeRotationAxis::Right:
			return TargetActor->GetActorRightVector();
		case EShakeRotationAxis::Up:
		default:
			return TargetActor->GetActorUpVector();
	}
}

void AAnomalyDoll::OnShaken()
{
	bHasBeenShaken = true;
	ShakeScore = 0.0f; // Reset the shake score when the doll is manually shaken
    UE_LOG(LogTemp, Log, TEXT("Doll has been shaken manually."));
}
void AAnomalyDoll::CheckBeenShaken(float DeltaTime)
{
	if(bHasBeenShaken){
		// Implement logic for when the doll has been shaken
	}
    else
    {
        AActor* MyOwner = RootComponent->GetOwner();
        if (!MyOwner || DeltaTime <= 0.0f)
        {
            return;
        }
        const FVector CurrentAxis = GetTargetAxisVector(MyOwner);
        
        // Calcola la variazione del vettore di direzione dell'asse
        const FVector AxisDelta = CurrentAxis - LastTrackedAxis;
        
        // Velocità di cambio dell'asse
        const FVector CurrentAngularVelocity = AxisDelta / DeltaTime;
        const float AngularSpeedDegrees = FMath::RadiansToDegrees(CurrentAngularVelocity.Size());

        bool bIsRotationInverted = false;

        if (AngularSpeedDegrees > 10.0f)
        {
            const FVector CurrentDir = CurrentAngularVelocity.GetSafeNormal();

            if (!LastAngularVelocity.IsNearlyZero())
            {
                const FVector LastDir = LastAngularVelocity.GetSafeNormal();
                const float Dot = FVector::DotProduct(CurrentDir, LastDir);

                // Se ruota abbastanza velocemente E sta invertendo il verso di rotazione
                if (AngularSpeedDegrees >= MinAngularSpeed && Dot <= InversionThreshold)
                {
                    bIsRotationInverted = true;
                }
            }

            LastAngularVelocity = CurrentAngularVelocity;
        }

        // Gestione dello Shake Score
        if (bIsRotationInverted)
        {
            ShakeScore += ShakeAccumulationRate * DeltaTime;
        }
        else
        {
            ShakeScore = FMath::Max(0.0f, ShakeScore - DecayRate * DeltaTime);
        }

        // Trigger Evento
        if (ShakeScore >= ShakeThreshold && !bHasBeenShaken)
        {
            bHasBeenShaken = true;
            OnShaken();
        }

        LastTrackedAxis = CurrentAxis;
    }
}

void AAnomalyDoll::OnTabletTicked()
{
}

void AAnomalyDoll::OnEndInteract()
{
    bHasBeenShaken = false;
    ShakeScore = 0.0f;
}

