// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnomaly.h"


void ABaseAnomaly::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsBeingInspected && MaxInspectionTime > 0.0f)
    {
        CurrentInspectionTime += DeltaTime;

        // Log the current inspection time for debugging
        UE_LOG(LogTemp, Warning, TEXT("Current Inspection Time: %f"), CurrentInspectionTime);

        if (CurrentInspectionTime >= MaxInspectionTime)
        {
            if(GEngine)
                GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Max inspection time reached for anomaly!"));	
        }
    }
}

void ABaseAnomaly::OnInteract()
{
    Super::OnInteract();

    CurrentInspectionTime = 0.0f; // Reset the current inspection time when interacting with the anomaly

    // Log the inspectionMaxTime value for debugging
    UE_LOG(LogTemp, Warning, TEXT("Max Inspection Time: %f"), MaxInspectionTime);

    // Additional logic for anomaly interaction can be added here
    bIsBeingInspected = true; // Set the flag to indicate that the anomaly is being inspected
}

void ABaseAnomaly::OnEndInteract()
{
    Super::OnEndInteract();
    bIsBeingInspected = false; // Reset the flag when interaction ends

}
