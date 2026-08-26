// Fill out your copyright notice in the Description page of Project Settings.

#include "Tablet.h"
#include "Variant_Horror/HorrorCharacter.h"
#include "Kismet/GameplayStatics.h"

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


void ATablet::OnEquipped()
{
    Super::OnEquipped();
    // Logic for when the tablet is equipped
    UE_LOG(LogTemp, Log, TEXT("Tablet equipped."));
    // You might want to show the tablet UI or enable interaction here
}

void ATablet::OnUnequipped()
{
    Super::OnUnequipped();
    // Logic for when the tablet is unequipped
    UE_LOG(LogTemp, Log, TEXT("Tablet unequipped."));
    // You might want to hide the tablet UI or disable interaction here
}

void ATablet::OnUsed()
{
    Super::OnUsed();
    // Logic for when the tablet is used
    UE_LOG(LogTemp, Log, TEXT("Tablet used."));
    // You might want to trigger some action or interaction here
}