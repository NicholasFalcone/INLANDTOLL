// Copyright Epic Games, Inc. All Rights Reserved.

#include "Variant_Horror/HorrorGameMode.h"
#include "GameFramework/PlayerController.h"
#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"

AHorrorGameMode::AHorrorGameMode()
{
	// stub
}

void AHorrorGameMode::ResetGameplay(APlayerController* TargetController)
{
    if (TargetController)
    {
        if (APawn* CurrentPawn = TargetController->GetPawn())
        {
            CurrentPawn->Destroy();
        }

        RestartPlayer(TargetController);
    }
}


AActor* AHorrorGameMode::ChoosePlayerStart_Implementation(AController* Player)
{

	if(bDebugging)
	{
		for (TActorIterator<APlayerStart> It(GetWorld()); It; ++It)
		{
			APlayerStart* PlayerStart = *It;
			if (PlayerStart && PlayerStart->PlayerStartTag == DebugPlayerStartTag)
			{
				return PlayerStart;
			}
		}
	}
	// stub
	return Super::ChoosePlayerStart_Implementation(Player);
}