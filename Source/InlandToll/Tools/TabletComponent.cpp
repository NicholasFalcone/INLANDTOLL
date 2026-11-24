// Fill out your copyright notice in the Description page of Project Settings.

#include "TabletComponent.h"
#include "Blueprint/UserWidget.h"
#include "UI/TableUI.h"
#include "UObject/UnrealType.h"

void UTabletComponent::InitializeWidgets(UTableUI* SheetWidget, UUserWidget* DefaultWidget, UWidget* TabletContainer)
{
    // OnSheetUpdate is declared in the existing Widget Blueprint, not in UTableUI.
    FScriptDelegate SheetUpdatedDelegate;
    SheetUpdatedDelegate.BindUFunction(this, GET_FUNCTION_NAME_CHECKED(UTabletComponent, NotifyTabletSheetUpdated));
    if (TabletWidget)
    {
        if (FMulticastDelegateProperty* Delegate = FindFProperty<FMulticastDelegateProperty>(TabletWidget->GetClass(), TEXT("OnSheetUpdate")))
        {
            Delegate->RemoveDelegate(SheetUpdatedDelegate, TabletWidget);
        }
    }

    TabletWidget = SheetWidget;
    DefaultPageWidget = DefaultWidget;
    TabletContainerWidget = TabletContainer;
    if (TabletWidget)
    {
        FMulticastDelegateProperty* Delegate = FindFProperty<FMulticastDelegateProperty>(TabletWidget->GetClass(), TEXT("OnSheetUpdate"));
        if (Delegate && Delegate->SignatureFunction && Delegate->SignatureFunction->NumParms == 0)
        {
            Delegate->AddDelegate(SheetUpdatedDelegate, TabletWidget);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Tablet sheet widget must define an OnSheetUpdate dispatcher with no parameters."));
        }
    }
    UpdateWidgetVisibility();
}

bool UTabletComponent::AreWidgetsReady() const
{
    return TabletWidget && DefaultPageWidget && TabletContainerWidget;
}

void UTabletComponent::SetOpen(bool bOpen)
{
    const bool bWasOpen = bIsOpen;
    bIsOpen = bOpen;
    UpdateWidgetVisibility();

    if (bIsOpen && !bWasOpen && TabletWidget)
    {
        TabletWidget->EnableSheet();
    }
}

void UTabletComponent::UpdateWidgetVisibility()
{
    if (TabletContainerWidget)
    {
        TabletContainerWidget->SetVisibility(bIsOpen ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
    }
    if (TabletWidget)
    {
        TabletWidget->SetVisibility(bIsOpen && bHasSheet ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    }
    if (DefaultPageWidget)
    {
        DefaultPageWidget->SetVisibility(bIsOpen && !bHasSheet ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
    }
}

bool UTabletComponent::HasSheet() const
{
    return bHasSheet;
}

int32 UTabletComponent::GetSheetScore() const
{
    if (!TabletWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot get tablet sheet score: sheet widget is unavailable."));
        return 0;
    }
    return TabletWidget->GetSheetScore();
}

bool UTabletComponent::UpdateAnomaly(const UAnomalyData* CurrentInspectionData)
{
    if (!CurrentInspectionData || !AreWidgetsReady())
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot update tablet: inspection data or tablet widgets are unavailable."));
        return false;
    }

    bHasSheet = true;
    TabletWidget->UpdateAnomaly(CurrentInspectionData);
    UpdateWidgetVisibility();
    OnTabletUpdateAnomaly.Broadcast(true);
    return true;
}

bool UTabletComponent::CanBeDelivered() const
{
    return TabletWidget && bHasSheet && TabletWidget->bSheetCompiled;
}

void UTabletComponent::RemoveSheet()
{
    if (!TabletWidget)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot remove tablet sheet: sheet widget is unavailable."));
        return;
    }

    TabletWidget->RemoveSheet();
    bHasSheet = false;
    UpdateWidgetVisibility();
    OnTabletUpdateAnomaly.Broadcast(false);
}

void UTabletComponent::NotifyTabletSheetUpdated()
{
    OnTabletSheetUpdated.Broadcast();
}

UTableUI* UTabletComponent::GetTabletWidget() const
{
    return TabletWidget;
}
