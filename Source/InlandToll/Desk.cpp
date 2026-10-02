#include "Desk.h"
#include "Components/ChildActorComponent.h"
#include "Components/WidgetComponent.h"
#include "BaseInteractable.h"
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

    SetupButtons();
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
        /// Trigger game over or appropriate response for reaching max errors
        OnMaxErrorsReached.Broadcast();
    }
			
}

void ADesk::RestartGame()
{
    if (InspectionManager)
    {
        InspectionManager->RestartGame();
    }
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
}

void ADesk::OnPrintDetailsButtonPressed()
{
    CurrentDayInspection++;
    OnDailyInspectionCountChanged.Broadcast(CurrentDayInspection);
    InspectionManager->SpawnNextAnomaly();
}

void ADesk::OnApproveButtonPressed()
{
    InspectionManager->PassCurrentInspectionDataToAnomaly();
}

void ADesk::OnRejectButtonPressed()
{
    InspectionManager->RejectCurrentInspectedAnomaly();
}


// Funzioni helper per ottenere l'istanza ABaseInteractable effettiva
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