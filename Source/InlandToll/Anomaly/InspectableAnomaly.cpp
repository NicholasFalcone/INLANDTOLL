// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectableAnomaly.h"
#include "HorrorCharacter.h"
#include "GeneralLightSwitch.h"
#include "Kismet/GameplayStatics.h"

void AInspectableAnomaly::BeginPlay()
{
    Super::BeginPlay();
    PlayerCharacter = Cast<AHorrorCharacter>(UGameplayStatics::GetActorOfClass(GetWorld(), AHorrorCharacter::StaticClass()));
    
    UVLight = Cast<AUVLight>(UGameplayStatics::GetActorOfClass(GetWorld(), AUVLight::StaticClass()));
    if(!UVLight){
        UE_LOG(LogTemp, Error, TEXT("UVLight actor not found in the scene."));
    }
    
    if(AGeneralLightSwitch* foundLightSwitch = Cast<AGeneralLightSwitch>(UGameplayStatics::GetActorOfClass(GetWorld(), AGeneralLightSwitch::StaticClass())))
    {
        UE_LOG(LogTemp, Warning, TEXT("GeneralLightSwitch found and bound to light state change."));
        foundLightSwitch->OnLightsToggled.AddDynamic(this, &AInspectableAnomaly::HandleLightStateChanged);
    }
    else{
        UE_LOG(LogTemp, Warning, TEXT("GeneralLightSwitch actor not found in the scene."));
    }
}

void AInspectableAnomaly::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if(bIsBeingInspected)
    {
        RotationDelta = FMath::Abs(StartAngle - LastAngle);
        LastAngle = GetActorRotation().Yaw;
        OnRotate(RotationDelta);
    }


    if (bIsBeingInspected && MaxInspectionTime > 0.0f)
    {
        CurrentInspectionTime += DeltaTime;

        // Log the current inspection time for debugging
        UE_LOG(LogTemp, Warning, TEXT("Current Inspection Time: %f"), CurrentInspectionTime);

        if (CurrentInspectionTime >= MaxInspectionTime)
        {
            if(GEngine)
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Max inspection time reached for anomaly!"));
                	
            if (PlayerCharacter)
            {
                PlayerCharacter->Die();
            }
        }
    }
    
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

bool AInspectableAnomaly::IsSpottedByUVLight()
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

void AInspectableAnomaly::OnStartSpottedByUVLight()
{
    OnSpottedByUVLight.Broadcast();
}

void AInspectableAnomaly::OnStopBeingSpottedByUVLight()
{
    OnStoppedBeingSpottedByUVLight.Broadcast();
}

void AInspectableAnomaly::HandleLightStateChanged(bool bIsLightOn)
{
    UE_LOG(LogTemp, Warning, TEXT("HandleLightStateChanged called with bIsLightOn: %s"), bIsLightOn ? TEXT("true") : TEXT("false"));
    OnLightStateChanged.Broadcast(bIsLightOn);
}

void AInspectableAnomaly::OnInteract()
{
    Super::OnInteract();

    CurrentInspectionTime = 0.0f; // Reset the current inspection time when interacting with the anomaly

    // Log the inspectionMaxTime value for debugging
    UE_LOG(LogTemp, Warning, TEXT("Max Inspection Time: %f"), MaxInspectionTime);

    // Additional logic for anomaly interaction can be added here
    bIsBeingInspected = true; // Set the flag to indicate that the anomaly is being inspected
    StartAngle = GetActorRotation().Yaw;
    LastAngle = StartAngle;
}

void AInspectableAnomaly::OnEndInteract()
{
    Super::OnEndInteract();
    bIsBeingInspected = false; // Reset the flag when interaction ends
}

void AInspectableAnomaly::OnInspectionProgress(float DeltaTime)
{
    // Log the inspection progress for debugging
    UE_LOG(LogTemp, Warning, TEXT("Inspection Progress: %f"), DeltaTime);
}

void AInspectableAnomaly::OnRotate(float delta)
{
    // Log the rotation delta for debugging
    // UE_LOG(LogTemp, Warning, TEXT("Rotation Delta: %f"), delta);
}
