#include "Desk.h"
#include "Components/ChildActorComponent.h"
#include "Components/WidgetComponent.h"
#include "DialogueManagerSubsystem.h"
#include "ST_DialogueLine.h"
#include "BaseInteractable.h"
#include "InspectionDayDataAsset.h"
#include "Tools/Tablet.h"
#include "Kismet/GameplayStatics.h"
#include "InspectionManager.h"

ADesk::ADesk()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // --- MONITORS (Widget Components) ---
    LeftMonitor = CreateDefaultSubobject<UWidgetComponent>(TEXT("LeftMonitor"));
    LeftMonitor->SetupAttachment(RootComponent);

    RightMonitor = CreateDefaultSubobject<UWidgetComponent>(TEXT("RightMonitor"));
    RightMonitor->SetupAttachment(RootComponent);
}

void ADesk::BeginPlay()
{
    Super::BeginPlay();
    InspectionManager = Cast<AInspectionManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AInspectionManager::StaticClass()));
    if (InspectionManager)
    {
        InspectionManager->OnInspectionEnded.AddDynamic(this, &ADesk::OnInspectionEnded);
        InspectionManager->OnInspectionError.AddDynamic(this, &ADesk::OnInspectionErrorIncreese);
        InspectionManager->OnAnomalyDetailsChange.AddDynamic(this, &ADesk::UpdateTabletDetails);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Desk could not find an InspectionManager."));
    }

    TabletInstance = Cast<ATablet>(UGameplayStatics::GetActorOfClass(GetWorld(), ATablet::StaticClass()));
    SetupButtons();
}

void ADesk::SetupButtons()
{
    if (ABaseInteractable* ApproveButton = GetApproveButton())
    {
        ApproveButton->OnInteractDelegate.AddDynamic(this, &ADesk::OnApproveButtonPressed);
    }

    if (ABaseInteractable* RejectButton = GetRejectButton())
    {
        RejectButton->OnInteractDelegate.AddDynamic(this, &ADesk::OnRejectButtonPressed);
    }

    if (ABaseInteractable* PrintDetailsButton = GetPrintDetailsButton())
    {
        PrintDetailsButton->OnInteractDelegate.AddDynamic(this, &ADesk::OnPrintDetailsButtonPressed);
    }
    
    if (ABaseInteractable* NewAnomalyButton = GetNewAnomalyButtonComponent())
    {
        NewAnomalyButton->OnInteractDelegate.AddDynamic(this, &ADesk::OnNewAnomalyButtonPressed);
    }

    if (ABaseInteractable* Button = GetPrintDetailsButton())
    {
        Button->bIsInteractable = false;
    }
    if (ABaseInteractable* Button = GetApproveButton())
    {
        Button->bIsInteractable = false;
    }
    if (ABaseInteractable* Button = GetRejectButton())
    {
        Button->bIsInteractable = false;
    }
    if (ABaseInteractable* Button = GetNewAnomalyButtonComponent())
    {
        Button->bIsInteractable = true;
    }
}

void ADesk::OnNewAnomalyButtonPressed()
{
    if (!InspectionManager)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot start an inspection because InspectionManager is unavailable."));
        return;
    }

    InspectionManager->SpawnNextAnomaly();
    if (!InspectionManager->CurrentInspectionPayload)
    {
        return;
    }

    InspectionPassed++;
    OnDailyInspectionCountChanged.Broadcast(InspectionPassed);

    if (ABaseInteractable* Button = GetPrintDetailsButton())
    {
        Button->ChangeInteractablePromptText(FText::FromString("Print"));
        Button->bIsInteractable = true;
    }
    if (ABaseInteractable* Button = GetApproveButton())
    {
        Button->bIsInteractable = false;
    }
    if (ABaseInteractable* Button = GetRejectButton())
    {
        Button->bIsInteractable = false;
    }
    if (ABaseInteractable* Button = GetNewAnomalyButtonComponent())
    {
        Button->bIsInteractable = false;
    }
}

void ADesk::OnPrintDetailsButtonPressed()
{
    ATablet* Tablet = GetTabletInstance();
    if (!Tablet)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot print inspection details because the tablet is unavailable."));
        return;
    }

    if(Tablet->HasSheet())
    {
        UE_LOG(LogTemp, Log, TEXT("Tablet has a sheet."));
        if(Tablet->CanBeDelivered())
        {
            UE_LOG(LogTemp, Log, TEXT("Delivering anomaly sheet."));
            DeliverAnomalySheet();
            if (ABaseInteractable* Button = GetPrintDetailsButton())
            {
                Button->bIsInteractable = false;
            }
            if (ABaseInteractable* Button = GetApproveButton())
            {
                Button->bIsInteractable = true;
            }
            if (ABaseInteractable* Button = GetRejectButton())
            {
                Button->bIsInteractable = true;
            }
            if (ABaseInteractable* Button = GetNewAnomalyButtonComponent())
            {
                Button->bIsInteractable = false;
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot deliver anomaly sheet. Tablet cannot be delivered."));

            			// Trigger dialogue associated with this anomaly comming up for inspection
			if (UDialogueManagerSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueManagerSubsystem>())
			{
                FST_DialogueLine CurrentDialogueLine = FST_DialogueLine();
                CurrentDialogueLine.SpeakerName = "Player";
                CurrentDialogueLine.DialogueText = "Cannot deliver anomaly sheet. You must complete the inspection first.";
				DialogueSubsystem->PlayDialogue(CurrentDialogueLine);
			}
        }
    }
    else
    {
        if (!CurrentAnomalyDetails)
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot print inspection details because no anomaly is active."));
            return;
        }

        Tablet->UpdateAnomaly(CurrentAnomalyDetails);
        if (ABaseInteractable* Button = GetPrintDetailsButton())
        {
            Button->bIsInteractable = true;
            Button->ChangeInteractablePromptText(FText::FromString("Submit"));
        }
        if (ABaseInteractable* Button = GetApproveButton())
        {
            Button->bIsInteractable = false;
        }
        if (ABaseInteractable* Button = GetRejectButton())
        {
            Button->bIsInteractable = false;
        }
        if (ABaseInteractable* Button = GetNewAnomalyButtonComponent())
        {
            Button->bIsInteractable = false;
        }
    }
}

void ADesk::OnApproveButtonPressed()
{
    if (!InspectionManager)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot approve inspection because InspectionManager is unavailable."));
        return;
    }

    InspectionManager->PassCurrentInspectionDataToAnomaly();
    if (ABaseInteractable* Button = GetPrintDetailsButton())
    {
        Button->ChangeInteractablePromptText(FText::FromString("Print"));
        Button->bIsInteractable = true;
    }
    if (ABaseInteractable* Button = GetApproveButton())
    {
        Button->bIsInteractable = false;
    }
    if (ABaseInteractable* Button = GetRejectButton())
    {
        Button->bIsInteractable = false;
    }
    if (ABaseInteractable* Button = GetNewAnomalyButtonComponent())
    {
        Button->bIsInteractable = false;
    }
}

void ADesk::OnRejectButtonPressed()
{
    if (!InspectionManager)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot reject inspection because InspectionManager is unavailable."));
        return;
    }

    InspectionManager->RejectCurrentInspectedAnomaly();
    if (ABaseInteractable* Button = GetPrintDetailsButton())
    {
        Button->ChangeInteractablePromptText(FText::FromString("Print"));
        Button->bIsInteractable = true;
    }
    if (ABaseInteractable* Button = GetApproveButton())
    {
        Button->bIsInteractable = false;
    }
    if (ABaseInteractable* Button = GetRejectButton())
    {
        Button->bIsInteractable = false;
    }
    if (ABaseInteractable* Button = GetNewAnomalyButtonComponent())
    {
        Button->bIsInteractable = false;
    }
}

void ADesk::DeliverAnomalySheet()
{
    if(GetTabletInstance() )
    {
        UE_LOG(LogTemp, Log, TEXT("Removed anomaly sheet from the tablet."));
        GetTabletInstance()->RemoveSheet();
    }
}

void ADesk::UpdateTabletDetails(const UAnomalyData* NewAnomalyDetails)
{
    // Implement the logic to update the tablet details based on the new anomaly details
    if (GetTabletInstance())
    {
        UE_LOG(LogTemp, Log, TEXT("Updating tablet details with new anomaly data."));
        CurrentAnomalyDetails = NewAnomalyDetails;
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Tablet instance is not valid."));
    }
}

void ADesk::OnInspectionEnded()
{
    InspectionPassed++;
}

int ADesk::LoadNextDay()
{
    CurrentDayReach += 1;
    CurrentErrors = 0;
    InspectionPassed = 0;
    OnDailyInspectionCountChanged.Broadcast(InspectionPassed);
    OnErrorCountChanged.Broadcast(CurrentErrors);
    if (InspectionManager)
    {
        InspectionManager->RestartGame();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot restart inspection day because InspectionManager is unavailable."));
    }

    if (ABaseInteractable* Button = GetPrintDetailsButton())
    {
        Button->bIsInteractable = false;
    }
    if (ABaseInteractable* Button = GetApproveButton())
    {
        Button->bIsInteractable = false;
    }
    if (ABaseInteractable* Button = GetRejectButton())
    {
        Button->bIsInteractable = false;
    }
    if (ABaseInteractable* Button = GetNewAnomalyButtonComponent())
    {
        Button->bIsInteractable = true;
    }
    return CurrentDayReach;
}

void ADesk::OnInspectionErrorIncreese()
{
    CurrentErrors++;
    OnErrorCountChanged.Broadcast(CurrentErrors);

    if(CurrentErrors < MaxErrorsAllowed)
    {
        UE_LOG(LogTemp, Warning, TEXT("Current errors: %d"), CurrentErrors);
        /// Update ui on left panel
    }
    else
    {
        OnMaxErrorsReached.Broadcast();
    }
}

void ADesk::RestartGame()
{
    InspectionPassed = 0;
    CurrentErrors = 0;
    OnDailyInspectionCountChanged.Broadcast(InspectionPassed);
    OnErrorCountChanged.Broadcast(CurrentErrors);
    if (InspectionManager)
    {
        InspectionManager->RestartGame();
    }
}

UAnomalyData* ADesk::GetInspectionDataForCurrentDay(int32 inspectionIndex)
{
    if (InspectionDataArray.IsValidIndex(CurrentDayReach))
    {
        return InspectionDataArray[CurrentDayReach].InspectionData.IsValidIndex(inspectionIndex) ? InspectionDataArray[CurrentDayReach].InspectionData[inspectionIndex] : nullptr;
    }
    return nullptr;
}

bool ADesk::IsDayOver(int32 inspectionIndex) const
{
    bool isEnded = CurrentDayReach >= InspectionDataArray.Num() || (InspectionDataArray.IsValidIndex(CurrentDayReach) && inspectionIndex >= InspectionDataArray[CurrentDayReach].InspectionData.Num());

    if(isEnded)
    {
        if (UDialogueManagerSubsystem* DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueManagerSubsystem>())
        {
            FST_DialogueLine CurrentDialogueLine = FST_DialogueLine();
            CurrentDialogueLine.SpeakerName = "Boss";
            CurrentDialogueLine.DialogueText = "It's time to go sleep.";
            DialogueSubsystem->PlayDialogue(CurrentDialogueLine);
        }
        if (ABaseInteractable* Button = GetPrintDetailsButton())
        {
            Button->bIsInteractable = false;
        }
        if (ABaseInteractable* Button = GetApproveButton())
        {
            Button->bIsInteractable = false;
        }
        if (ABaseInteractable* Button = GetRejectButton())
        {
            Button->bIsInteractable = false;
        }
        if (ABaseInteractable* Button = GetNewAnomalyButtonComponent())
        {
            Button->bIsInteractable = false;
        }
        OnDailyInspectionLimitReached.Broadcast();
    }
    else{
        OnDailyInspectionCountChanged.Broadcast(InspectionPassed);
    }

    return isEnded;
}

#pragma region HelperFunctions
// Funzioni helper per ottenere l'istanza ABaseInteractable effettiva
ABaseInteractable* ADesk::GetNewAnomalyButtonComponent() const
{
    return NewAnomalyButtonComponent;
}

ABaseInteractable* ADesk::GetApproveButton() const
{
    return ApproveButtonComponent;
}

ABaseInteractable* ADesk::GetRejectButton() const
{
    return RejectButtonComponent;
}

ABaseInteractable* ADesk::GetPrintDetailsButton() const
{
    return PrintDetailsButtonComponent;
}

ATablet* ADesk::GetTabletInstance()
{
    if(TabletInstance)
        return TabletInstance;
    else{
        TabletInstance = Cast<ATablet>(UGameplayStatics::GetActorOfClass(GetWorld(), ATablet::StaticClass()));
        if(TabletInstance)
            return TabletInstance;
    }
    return nullptr;
}

int32 ADesk::CurrentDayReachGetter() const
{
    return CurrentDayReach;
}

#pragma endregion
