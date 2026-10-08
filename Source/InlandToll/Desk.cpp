#include "Desk.h"
#include "Components/ChildActorComponent.h"
#include "Components/WidgetComponent.h"
#include "DialogueManagerSubsystem.h"
#include "ST_DialogueLine.h"
#include "BaseInteractable.h"
#include "Tools/Tablet.h"
#include "Kismet/GameplayStatics.h"
#include "InspectionManager.h"

ADesk::ADesk()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
    // --- BUTTONS (Child Actor Components) ---
    PrintDetailsButtonComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("PrintDetailsButton"));
    PrintDetailsButtonComponent->SetupAttachment(RootComponent);
    PrintDetailsButtonComponent->SetChildActorClass(ABaseInteractable::StaticClass());

    ApproveButtonComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("ApproveButton"));
    ApproveButtonComponent->SetupAttachment(RootComponent);
    ApproveButtonComponent->SetChildActorClass(ABaseInteractable::StaticClass());

    RejectButtonComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("RejectButton"));
    RejectButtonComponent->SetupAttachment(RootComponent);
    RejectButtonComponent->SetChildActorClass(ABaseInteractable::StaticClass());

    NewAnomalyButtonComponent = CreateDefaultSubobject<UChildActorComponent>(TEXT("NewAnomalyButton"));
    NewAnomalyButtonComponent->SetupAttachment(RootComponent);
    NewAnomalyButtonComponent->SetChildActorClass(ABaseInteractable::StaticClass());


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
    CurrentDayInspection++;
    OnDailyInspectionCountChanged.Broadcast(CurrentDayInspection);
    if (InspectionManager)
    {
        InspectionManager->SpawnNextAnomaly();
    }
    GetPrintDetailsButton()->bIsInteractable = true;
    GetPrintDetailsButton()->ChangeInteractablePromptText(FText::FromString("Print"));
    GetApproveButton()->bIsInteractable = false;
    GetRejectButton()->bIsInteractable = false;
    GetNewAnomalyButtonComponent()->bIsInteractable = false;
}

void ADesk::OnPrintDetailsButtonPressed()
{
    UE_LOG(LogTemp, Log, TEXT("Print Details button pressed. %s \n anomaly details %s"), GetTabletInstance() ? (GetTabletInstance()->HasSheet() ? TEXT("Has Sheet") : TEXT("No Sheet")) : TEXT("No Tablet Instance"), *CurrentAnomalyDetails.InspectionName.ToString());
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
    GetPrintDetailsButton()->bIsInteractable = true;
    GetApproveButton()->bIsInteractable = false;
    GetRejectButton()->bIsInteractable = false;
    GetNewAnomalyButtonComponent()->bIsInteractable = false;
}

void ADesk::OnRejectButtonPressed()
{
    InspectionManager->RejectCurrentInspectedAnomaly();
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

void ADesk::UpdateTabletDetails(const FInspectionData& NewAnomalyDetails)
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
    CurrentDayInspection++;
    OnDailyInspectionCountChanged.Broadcast(CurrentDayInspection);
    if(CurrentDayInspection >= InspectionToDailyReach)
    {
        OnDailyInspectionLimitReached.Broadcast();
    }
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
    CurrentDayInspection = 0;
    CurrentErrors = 0;
    OnDailyInspectionCountChanged.Broadcast(CurrentDayInspection);
    OnErrorCountChanged.Broadcast(CurrentErrors);
    if (InspectionManager)
    {
        InspectionManager->RestartGame();
    }
}


// Funzioni helper per ottenere l'istanza ABaseInteractable effettiva
ABaseInteractable* ADesk::GetNewAnomalyButtonComponent() const
{
    return NewAnomalyButtonComponent ? Cast<ABaseInteractable>(NewAnomalyButtonComponent->GetChildActor()) : nullptr;
}

ABaseInteractable* ADesk::GetApproveButton() const
{
    return ApproveButtonComponent ? Cast<ABaseInteractable>(ApproveButtonComponent->GetChildActor()) : nullptr;
}

ABaseInteractable* ADesk::GetRejectButton() const
{
    return RejectButtonComponent ? Cast<ABaseInteractable>(RejectButtonComponent->GetChildActor()) : nullptr;
}

ABaseInteractable* ADesk::GetPrintDetailsButton() const
{
    return PrintDetailsButtonComponent ? Cast<ABaseInteractable>(PrintDetailsButtonComponent->GetChildActor()) : nullptr;
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
