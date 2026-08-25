// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAnomaly.h"

void ABaseAnomaly::OnInteract()
{
    Super::OnInteract();

    CurrentInspectionTime = 0.0f; // Reset the current inspection time when interacting with the anomaly

    // Log the inspectionMaxTime value for debugging
    UE_LOG(LogTemp, Warning, TEXT("Max Inspection Time: %f"), MaxInspectionTime);

    // Additional logic for anomaly interaction can be added here
}
