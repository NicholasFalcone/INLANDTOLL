// Fill out your copyright notice in the Description page of Project Settings.

#include "Tablet.h"

ATablet::ATablet()
{
    // Constructor logic if needed
    TabletWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TabletWidgetComponent"));
    TabletWidgetComponent->SetWidgetClass(TabletWidgetClass);
    TabletWidgetComponent->SetupAttachment(RootComponent);
}


void ATablet::UpdateAnomaly(const FInspectionData& currentInspectionData)
{
    // Assuming the Tablet has a method to update its display based on the inspection data
    // You would implement the logic here to update the tablet's UI or state
    UE_LOG(LogTemp, Log, TEXT("Updating Tablet with new anomaly data."));
    UTableUI* TableWidget = Cast<UTableUI>(TabletWidgetComponent->GetUserWidgetObject());
    if (TableWidget)
    {
        TableWidget->UpdateAnomaly(currentInspectionData);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TabletWidgetComponent does not have a valid UTableUI instance."));
    }
}
