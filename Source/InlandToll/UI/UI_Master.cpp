// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Master.h"

void UUI_Master::SetupCharacter(AHorrorCharacter* HorrorCharacter)
{
    HorrorCharacter->OnPlayerDied.AddDynamic(this, &UUI_Master::HandleGameOver);
    HorrorCharacter->OnPlayerGettingCut.AddDynamic(this, &UUI_Master::HandlePlayerGettingCut);
}

void UUI_Master::HandleGameOver()
{
    // Call the Blueprint event to handle game over
    BP_OnGameOver(E_LOSE_REASON::LOSE_REASON_ANOMALY);
}

void UUI_Master::SetupManager(AInspectionManager* InspectionManager)
{
    // Implement logic to set up the inspection car manager in the UI
    InspectionManager->OnErrorCountChanged.AddDynamic(this, &UUI_Master::HandleErrorCountChanged);
    InspectionManager->OnMaxErrorsReached.AddDynamic(this, &UUI_Master::HandleMaxErrorsReached);
    InspectionManager->OnDailyInspectionLimitReached.AddDynamic(this, &UUI_Master::HandleDailyInspectionLimitReached);
    InspectionManager->OnDailyInspectionCountChanged.AddDynamic(this, &UUI_Master::HandleDailyInspectionCountChanged);
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

void UUI_Master::HandlePlayerGettingCut()
{
    // Call the Blueprint event to handle player getting cut
    BP_OnPlayerGettingCut();
}

void UUI_Master::HandleDailyInspectionLimitReached()
{
    // Call the Blueprint event to handle daily inspection limit reached
    BP_OnDailyInspectionLimitReached();
}

void UUI_Master::HandleDailyInspectionCountChanged(int32 NewDailyInspectionCount)
{
    // Call the Blueprint event to update the daily inspection count in the UI
    BP_OnDailyInspectionCountChanged(NewDailyInspectionCount);
}
