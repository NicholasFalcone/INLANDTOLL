// Fill out your copyright notice in the Description page of Project Settings.


#include "UI_Master.h"
#include "Desk.h"
#include "Variant_Horror/HorrorCharacter.h"

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

void UUI_Master::SetupManager(ADesk* DeskInstance)
{
    // Implement logic to set up the desk instance in the UI
    DeskInstance->OnErrorCountChanged.AddDynamic(this, &UUI_Master::HandleErrorCountChanged);
    DeskInstance->OnMaxErrorsReached.AddDynamic(this, &UUI_Master::HandleMaxErrorsReached);
    DeskInstance->OnDailyInspectionLimitReached.AddDynamic(this, &UUI_Master::HandleDailyInspectionLimitReached);
    DeskInstance->OnDailyInspectionCountChanged.AddDynamic(this, &UUI_Master::HandleDailyInspectionCountChanged);
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

void UUI_Master::HandleLoadNextDay()
{
    // Call the Blueprint event to handle loading the next day
    BP_OnLoadNextDay();
}
