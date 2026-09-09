// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "HorrorGameMode.generated.h"

/**
 *  Simple GameMode for a first person horror game
 */
UCLASS(abstract)
class INLANDTOLL_API AHorrorGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
private:
	const FString DebugPlayerStartTag = "DebugPlayerStart";

protected:
	UPROPERTY(EditAnywhere, Category = "Horror|GameMode")
	bool bDebugging = true;

public:

	/** Constructor */
	AHorrorGameMode();

	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
};
