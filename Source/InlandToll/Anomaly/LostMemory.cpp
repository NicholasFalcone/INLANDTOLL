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

    float remainder = FMath::Fmod(FMath::Abs(delta), 180.0f);

    if(remainder > MaxAngleTolerance)
    {
        // Implement logic for when the rotation exceeds the maximum angle tolerance
        MonsterDistance = FMath::Min(MonsterDistance + .1, MaxMonsterDistance);
        
        if (GEngine)
        {
            // AddOnScreenDebugMessage(Key, TimeToDisplay, Color, Message)
            GEngine->AddOnScreenDebugMessage(
                -1,                             // Key (-1 evita che il messaggio sovrascriva quello precedente)
                5.0f,                           // Tempo di permanenza a schermo in secondi
                FColor::Green,                  // Colore del testo
                FString::Printf(TEXT("DEBUG: MonsterDistance %f at MaxMonsterDistance %f you DIE"), MonsterDistance, MaxMonsterDistance) // Formattazione stringa
            );
        }

        if(MonsterDistance >= MaxMonsterDistance)
        {
            if (PlayerCharacter)
            {
                PlayerCharacter->Die();
            }
        }
    }
}