// Fill out your copyright notice in the Description page of Project Settings.


#include "Anomaly/CursedSword.h"


void ACursedSword::OnRotate(float delta)
{
    Super::OnRotate(delta);
    // Custom logic for the cursed sword rotation can be added here
    if(delta >= CuttingAngle)
    {
        // Implement the logic for when the rotation delta exceeds the cutting angle
        UE_LOG(LogTemp, Warning, TEXT("Cutting angle exceeded: %f"), delta);
    }
}
