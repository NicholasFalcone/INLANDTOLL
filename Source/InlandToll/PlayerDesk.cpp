// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerDesk.h"
#include "BaseInteractable.h"
#include "Kismet/GameplayStatics.h"
#include "DialogueManagerSubsystem.h"
#include "Desk.h"
#include "Tools/TabletComponent.h"
#include "Variant_Horror/HorrorCharacter.h"
#include "Components/ChildActorComponent.h"
#include "Components/WidgetComponent.h"

namespace
{
    ABaseInteractable* ResolvePlayerDeskButton(const UChildActorComponent* Component)
    {
        ABaseInteractable* Button = Component ? Cast<ABaseInteractable>(Component->GetChildActor()) : nullptr;
        if (!Button)
        {
            UE_LOG(LogTemp, Error, TEXT("%s must have a Child Actor Class derived from ABaseInteractable."),
                *GetNameSafe(Component));
        }
        return Button;
    }
}

// Sets default values
APlayerDesk::APlayerDesk()
{
    // Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    PowerButtonInteractable = CreateDefaultSubobject<UChildActorComponent>(TEXT("PowerButtonInteractable"));
    PowerButtonInteractable->SetupAttachment(RootComponent);

    PrintDetailsButtonInteractable = CreateDefaultSubobject<UChildActorComponent>(TEXT("PrintDetailsButtonInteractable"));
    PrintDetailsButtonInteractable->SetupAttachment(RootComponent);

    // --- MONITORS (Widget Components) ---
    LeftMonitor = CreateDefaultSubobject<UWidgetComponent>(TEXT("LeftMonitor"));
    LeftMonitor->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlayerDesk::BeginPlay()
{
    Super::BeginPlay();

    GeneralDeskInstance = Cast<ADesk>(UGameplayStatics::GetActorOfClass(GetWorld(), ADesk::StaticClass()));

    if (GeneralDeskInstance)
    {
        GeneralDeskInstance->OnNextDayLoaded.AddDynamic(this, &APlayerDesk::OnNextDayBegin);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerDesk could not find a Desk."));
    }

    PowerButtonComponent = ResolvePlayerDeskButton(PowerButtonInteractable);
    PrintDetailsButtonComponent = ResolvePlayerDeskButton(PrintDetailsButtonInteractable);

    if (PowerButtonComponent)
    {
        PowerButtonComponent->OnInteractDelegate.AddDynamic(this, &APlayerDesk::OnPowerButtonPressed);
        PowerButtonComponent->bIsInteractable = true;
    }
    else{
        UE_LOG(LogTemp, Error, TEXT("Power button component is not set up correctly."));
    }

    if (PrintDetailsButtonComponent)
    {
        PrintDetailsButtonComponent->OnInteractDelegate.AddDynamic(this, &APlayerDesk::OnPrintDetailsButtonPressed);
        PrintDetailsButtonComponent->bIsInteractable = false;
    }
    else{
        UE_LOG(LogTemp, Error, TEXT("Print Details button component is not set up correctly."));
    }
}

void APlayerDesk::OnNextDayBegin(int day)
{
    CurrentQuestionaryIndex = FMath::Max(0, day - 1);

    if (PowerButtonComponent)
    {
        PowerButtonComponent->bIsInteractable = true;
    }

    if (PrintDetailsButtonComponent)
    {
        PrintDetailsButtonComponent->bIsInteractable = false;
    }

    OnNextDayBeginEvent.Broadcast();
}

void APlayerDesk::OnDayshiftEnded()
{
    if (PowerButtonComponent)
    {
        PowerButtonComponent->bIsInteractable = false;
    }

    if (PrintDetailsButtonComponent)
    {
        PrintDetailsButtonComponent->bIsInteractable = false;
    }
}

void APlayerDesk::OnPowerButtonPressed()
{
    UE_LOG(LogTemp, Log, TEXT("Power button pressed."));
    if (PowerButtonComponent)
    {
        PowerButtonComponent->bIsInteractable = false;
    }

    CurrentAnomalyDetails = DailyQuestionary.InspectionData.IsValidIndex(CurrentQuestionaryIndex)
                                ? DailyQuestionary.InspectionData[CurrentQuestionaryIndex]
                                : nullptr;

    if (!CurrentAnomalyDetails)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot load questionary data for day index %d. DailyQuestionary contains %d entries."),
               CurrentQuestionaryIndex,
               DailyQuestionary.InspectionData.Num());
        if (PrintDetailsButtonComponent)
        {
            PrintDetailsButtonComponent->bIsInteractable = false;
        }
        return;
    }

    UE_LOG(LogTemp, Log, TEXT("Loaded questionary data for day %d: %s"),
           CurrentQuestionaryIndex + 1,
           *CurrentAnomalyDetails->InspectionName.ToString());

    if (PrintDetailsButtonComponent)
    {
        PrintDetailsButtonComponent->bIsInteractable = true;
    }
}

int32 APlayerDesk::GetCurrentDayReach() const
{
    return GeneralDeskInstance ? GeneralDeskInstance->CurrentDayReach : 0;
}

UTabletComponent* APlayerDesk::GetTabletInstance()
{
    AHorrorCharacter* Player = Cast<AHorrorCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    TabletInstance = Player ? Player->MyTablet : nullptr;
    return TabletInstance;
}

void APlayerDesk::DeliverAnomalySheet()
{
    if (GetTabletInstance())
    {
        UE_LOG(LogTemp, Log, TEXT("Removed anomaly sheet from the tablet."));
        GetTabletInstance()->RemoveSheet();
        OnSurvayCompleted.Broadcast();
    }
}

void APlayerDesk::OnPrintDetailsButtonPressed()
{
    UE_LOG(LogTemp, Log, TEXT("Print Details button pressed."));
    
    UTabletComponent* Tablet = GetTabletInstance();
    if (!Tablet)
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot print questionnaire: tablet is unavailable."));
        return;
    }
    if (Tablet->HasSheet())
    {
        UE_LOG(LogTemp, Log, TEXT("Tablet has a sheet."));
        if (Tablet->CanBeDelivered())
        {
            UE_LOG(LogTemp, Log, TEXT("Delivering anomaly sheet."));
            DeliverAnomalySheet();
            /// Update button states after delivering the anomaly sheet.
            if (PrintDetailsButtonComponent)
            {
                PrintDetailsButtonComponent->bIsInteractable = false;
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Cannot deliver anomaly sheet. Tablet cannot be delivered."));

            // Trigger dialogue associated with this anomaly comming up for inspection
            if (UDialogueManagerSubsystem *DialogueSubsystem = GetGameInstance()->GetSubsystem<UDialogueManagerSubsystem>())
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
            UE_LOG(LogTemp, Warning, TEXT("CurrentAnomalyDetails is null: no anomaly data to print."));
            return;
        }
        if (!Tablet->UpdateAnomaly(CurrentAnomalyDetails))
        {
            return;
        }
        OnPrintButton2Pressed.Broadcast();
        if (PrintDetailsButtonComponent)
        {
            PrintDetailsButtonComponent->ChangeInteractablePromptText(FText::FromString("Submit"));
        }
    }
}

// Called every frame
void APlayerDesk::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
