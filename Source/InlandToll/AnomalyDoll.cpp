// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "AnomalyDoll.h"

AAnomalyDoll::AAnomalyDoll()
{
    ShakeAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AudioComponent"));
    ShakeAudioComponent->SetupAttachment(RootComponent);
    ShakeAudioComponent->bAutoActivate = false; // Prevent the audio from playing automatically on begin play
    ShakeAudioComponent->bIsUISound = false; // Ensure the audio is treated as a 3D sound in the world
    ShakeAudioComponent->bAllowSpatialization = true; // Enable 3D spatialization for the audio component
    ShakeAudioComponent->SetRelativeLocation(FVector::ZeroVector); // Ensure the audio component is at the root location
    ShakeAudioComponent->SetRelativeRotation(FRotator::ZeroRotator); // Ensure the audio component has no relative rotation
}

void AAnomalyDoll::BeginPlay()
{
	Super::BeginPlay();
	bHasBeenShaken = false;
	ShakeScore = 0.0f;
    if (ShakeAudioComponent && ShakeSound)
    {
        ShakeAudioComponent->SetSound(ShakeSound);
    }

    UVLight = Cast<AUVLight>(UGameplayStatics::GetActorOfClass(GetWorld(), AUVLight::StaticClass()));
    if(!UVLight){
        UE_LOG(LogTemp, Error, TEXT("UVLight actor not found in the scene."));
    }
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
	ShakeScore = 0.0f; // Reset the shake score when the doll is manually shaken
    UE_LOG(LogTemp, Log, TEXT("Doll has been shaken manually."));
    if (ShakeAudioComponent && ShakeSound)
    {
        ShakeAudioComponent->Play();
    }
	bHasBeenShaken = false;
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
    UE_LOG(LogTemp, Log, TEXT("Doll has been ticked by the tablet."));
}

void AAnomalyDoll::OnEndInteract()
{
    ShakeScore = 0.0f;
    bHasBeenShaken = false;
}

