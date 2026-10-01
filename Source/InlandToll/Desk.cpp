#include "Desk.h"
#include "Components/ChildActorComponent.h"
#include "Components/WidgetComponent.h"
#include "BaseInteractable.h"

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