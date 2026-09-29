// Fill out your copyright notice in the Description page of Project Settings.


#include "Anomaly/LostMemory.h"

void ALostMemory::BeginPlay()
{
	Super::BeginPlay();
}

void ALostMemory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ALostMemory::OnRotate(float delta)
{
	// Implement rotation logic here
    Super::OnRotate(delta);

    if(FMath::Abs(delta) > MaxAngleTolerance)
    {
        // Implement logic for when the rotation exceeds the maximum angle tolerance
        MonsterDistance = FMath::Min(MonsterDistance + .1, MaxMonsterDistance);
    }
}