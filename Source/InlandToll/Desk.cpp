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
    InspectionManager->OnInspectionEnded.AddDynamic(this, &ADesk::OnInspectionEnded);
    InspectionManager->OnInspectionError.AddDynamic(this, &ADesk::OnInspectionErrorIncreese);
    InspectionManager->OnAnomalyDetailsChange.AddDynamic(this, &ADesk::UpdateTabletDetails);
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

    GetPrintDetailsButton()->bIsInteractable = false;
    GetApproveButton()->bIsInteractable = false;
    GetRejectButton()->bIsInteractable = false;
    GetNewAnomalyButtonComponent()->bIsInteractable = true;
}

void ADesk::OnNewAnomalyButtonPressed()
{
    InspectionPassed++;
    OnDailyInspectionCountChanged.Broadcast(InspectionPassed);
    if (InspectionManager)
    {
        InspectionManager->SpawnNextAnomaly();
    }
    GetPrintDetailsButton()->ChangeInteractablePromptText(FText::FromString("Print"));
    GetPrintDetailsButton()->bIsInteractable = true;
    GetApproveButton()->bIsInteractable = false;
    GetRejectButton()->bIsInteractable = false;
    GetNewAnomalyButtonComponent()->bIsInteractable = false;
}

void ADesk::OnPrintDetailsButtonPressed()
{
    if(GetTabletInstance()->HasSheet())
    {
        UE_LOG(LogTemp, Log, TEXT("Tablet has a sheet."));
        if(GetTabletInstance()->CanBeDelivered())
        {
            UE_LOG(LogTemp, Log, TEXT("Delivering anomaly sheet."));
            DeliverAnomalySheet();
            /// Update button states after delivering the anomaly sheet.
            GetPrintDetailsButton()->bIsInteractable = false;
            GetApproveButton()->bIsInteractable = true;
            GetRejectButton()->bIsInteractable = true;
            GetNewAnomalyButtonComponent()->bIsInteractable = false;
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
        GetTabletInstance()->UpdateAnomaly(CurrentAnomalyDetails);
        GetPrintDetailsButton()->bIsInteractable = true;
        GetApproveButton()->bIsInteractable = false;
        GetRejectButton()->bIsInteractable = false;
        GetNewAnomalyButtonComponent()->bIsInteractable = false;
        GetPrintDetailsButton()->ChangeInteractablePromptText(FText::FromString("Submit"));
    }
}

void ADesk::OnApproveButtonPressed()
{
    InspectionManager->PassCurrentInspectionDataToAnomaly();
    GetPrintDetailsButton()->ChangeInteractablePromptText(FText::FromString("Print"));
    GetPrintDetailsButton()->bIsInteractable = true;
    GetApproveButton()->bIsInteractable = false;
    GetRejectButton()->bIsInteractable = false;
    GetNewAnomalyButtonComponent()->bIsInteractable = false;
}

void ADesk::OnRejectButtonPressed()
{
    InspectionManager->RejectCurrentInspectedAnomaly();
    GetPrintDetailsButton()->ChangeInteractablePromptText(FText::FromString("Print"));
    GetPrintDetailsButton()->bIsInteractable = true;
    GetApproveButton()->bIsInteractable = false;
    GetRejectButton()->bIsInteractable = false;
    GetNewAnomalyButtonComponent()->bIsInteractable = false;
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
    CurrentDayReach++;
    CurrentErrors = 0;
    InspectionPassed = 0;
    OnDailyInspectionCountChanged.Broadcast(InspectionPassed);
    OnErrorCountChanged.Broadcast(CurrentErrors);
    InspectionManager->RestartGame();
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
        return InspectionDataArray[CurrentDayReach].InspectionData.IsValidIndex(inspectionIndex) ? InspectionDataArray[CurrentDayReach].InspectionData[inspectionIndex]++ : nullptr;
    }
    return nullptr;
}

bool ADesk::IsDayOver(int32 inspectionIndex) const
{
    bool isEnded = CurrentDayReach >= InspectionDataArray.Num() || (InspectionDataArray.IsValidIndex(CurrentDayReach) && inspectionIndex >= InspectionDataArray[CurrentDayReach].InspectionData.Num());

    if(isEnded)
    {
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

#pragma endregion
