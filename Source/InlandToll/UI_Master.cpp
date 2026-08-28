// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Master.h"

void UUI_Master::SetupCharacter(AHorrorCharacter* HorrorCharacter)
{
    HorrorCharacter->OnPlayerDied.AddDynamic(this, &UUI_Master::HandleGameOver);
}

void UUI_Master::HandleGameOver()
{
    // Call the Blueprint event to handle game over
    BP_OnGameOver(E_LOSE_REASON::LOSE_REASON_ANOMALY);
}

void UUI_Master::SetupManager(AInspectionManager* CarManager)
{
    // Implement logic to set up the inspection car manager in the UI
    CarManager->OnErrorCountChanged.AddDynamic(this, &UUI_Master::HandleErrorCountChanged);
    CarManager->OnMaxErrorsReached.AddDynamic(this, &UUI_Master::HandleMaxErrorsReached);
}

void UUI_Master::HandleErrorCountChanged(int32 NewErrorCount)
{
    // Call the Blueprint event to update the UI
    BP_UserErrorCountChanged(NewErrorCount);
}

void UUI_Master::HandleMaxErrorsReached()
{
    // Call the Blueprint event to handle game over
    BP_OnGameOver(E_LOSE_REASON::LOSE_REASON_ERRORS);
}