// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractableAnomaly.h"
#include "HorrorCharacter.h"
#include "Kismet/GameplayStatics.h"


void AInteractableAnomaly::BeginPlay()
{
    Super::BeginPlay();
    PlayerCharacter = Cast<AHorrorCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AHorrorCharacter::StaticClass()));
    UVLight = Cast<AUVLight>(UGameplayStatics::GetActorOfClass(GetWorld(), AUVLight::StaticClass()));
    if(!UVLight){
        UE_LOG(LogTemp, Error, TEXT("UVLight actor not found in the scene."));
    }
}


void AInteractableAnomaly::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (IsSpottedByUVLight())
    {
        // Implement any logic that should occur when the doll is spotted by the UVLight
        if(bIsBeingSpottedByUVLight == false)
            OnStartSpottedByUVLight();
        bIsBeingSpottedByUVLight = true;
    }
    else if(bIsBeingSpottedByUVLight)
    {
        OnStopBeingSpottedByUVLight();
        bIsBeingSpottedByUVLight = false;
    }
}


bool AInteractableAnomaly::IsSpottedByUVLight()
{
    if(UVLight && UVLight->IsInUse)
    {
        /// Implement the logic to determine if the doll is currently spotted by the UVLight.
        FVector UVLightLocation = UVLight->GetActorLocation(); // Example of getting the UVLight's location
        FVector DollLocation = GetActorLocation(); // Example of getting the doll's location
        FVector DirectionToDoll = (DollLocation - UVLightLocation).GetSafeNormal(); // Direction from UVLight to the doll
        float DistanceToDoll = FVector::Dist(UVLightLocation, DollLocation); // Distance between UVLight and the doll
        float dir = FVector::DotProduct(UVLight->GetActorForwardVector(), DirectionToDoll); // Example of calculating the direction cosine between UVLight's forward vector and the direction to the doll
        if (dir > 0.9f && DistanceToDoll < 1000.0f) // Example condition for being spotted
        {
            return true;
        }
        return false;        
    }
    return false;
}

void AInteractableAnomaly::OnStartSpottedByUVLight()
{
    OnSpottedByUVLight.Broadcast();
}

void AInteractableAnomaly::OnStopBeingSpottedByUVLight()
{
    OnStoppedBeingSpottedByUVLight.Broadcast();
}

