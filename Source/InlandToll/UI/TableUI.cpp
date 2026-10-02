// Fill out your copyright notice in the Description page of Project Settings.


#include "TableUI.h"


void UTableUI::UpdateAnomaly(const FInspectionData& currentInspectionData)
{
    bSheetCompiled = false;
    // Call the Blueprint-implemented event to update the UI
    CallUpdateAnomaly(currentInspectionData);
}

void UTableUI::RemoveSheet()
{
    OnSheetRemoved.Broadcast();
    bSheetCompiled = false;
}