// Fill out your copyright notice in the Description page of Project Settings.


#include "Computer.h"
#include "InspectionProp.h"

AComputer::AComputer()
{
    ComputerScreenWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("ComputerScreenWidget"));
    ComputerScreenWidget->SetupAttachment(RootComponent);
}

void AComputer::BeginPlay()
{
    Super::BeginPlay();

    if (ComputerScreenWidget)
    {
        ComputerScreenWidgetInstance = Cast<UComputerWidget>(ComputerScreenWidget->GetUserWidgetObject());
        
        if (ComputerScreenWidgetInstance)
        {
            UE_LOG(LogTemp, Warning, TEXT("ComputerScreenWidgetInstance found!"));
            RefreshBannedListUI();
        }
        else{
            UE_LOG(LogTemp, Warning, TEXT("ComputerScreenWidgetInstance is not set!"));
        }
    }
    else{
        UE_LOG(LogTemp, Warning, TEXT("ComputerScreenWidget is not set!"));
    }
}

void AComputer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void AComputer::RefreshBannedListUI()
{
    if (!ComputerScreenWidgetInstance) return;

    TArray<FInspectionPropDetails> AllBannedProps;

    for (UBannedList* List : BannedLists)
    {
        if (List)
        {
            for (TSubclassOf<AInspectionProp> PropClass : List->BannedItems)
            {
                if (PropClass)
                {
                    if (AInspectionProp* DefaultProp = PropClass.GetDefaultObject())
                    {
                        AllBannedProps.Add(DefaultProp->PropDetails);
                    }
                }
            }
        }
    }

    ComputerScreenWidgetInstance->UpdateBannedList(AllBannedProps);
}