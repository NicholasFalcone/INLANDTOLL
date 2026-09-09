// Copyright Epic Games, Inc. All Rights Reserved.

#include "EngineUtils.h"
#include "GameFramework/PlayerStart.h"
#include "Variant_Horror/HorrorGameMode.h"

AHorrorGameMode::AHorrorGameMode()
{
	// stub
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