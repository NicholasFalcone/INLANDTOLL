// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerDesk.h"
#include "Kismet/GameplayStatics.h"
#include "DialogueManagerSubsystem.h"
#include "Desk.h"
#include "Tools/Tablet.h"

// Sets default values
APlayerDesk::APlayerDesk()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    // --- MONITORS (Widget Components) ---
    LeftMonitor = CreateDefaultSubobject<UWidgetComponent>(TEXT("LeftMonitor"));
    LeftMonitor->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void APlayerDesk::BeginPlay()
{
	Super::BeginPlay();

    TabletInstance = Cast<ATablet>(UGameplayStatics::GetActorOfClass(GetWorld(), ATablet::StaticClass()));
	
	GeneralDeskInstance = Cast<ADesk>(UGameplayStatics::GetActorOfClass(GetWorld(), ADesk::StaticClass()));

	GeneralDeskInstance->OnDailyInspectionLimitReached.AddDynamic(this, &APlayerDesk::OnDayshiftEnded);

    if (PowerButtonComponent)
    {
        PowerButtonComponent->OnInteractDelegate.AddDynamic(this, &APlayerDesk::OnPowerButtonPressed);
    }
	
    if (PrintDetailsButtonComponent)
    {
        PrintDetailsButtonComponent->OnInteractDelegate.AddDynamic(this, &APlayerDesk::OnPrintDetailsButtonPressed);
    }
}

void APlayerDesk::OnDayshiftEnded()
{
    // Handle the end of the day shift here.
}

void APlayerDesk::OnPowerButtonPressed()
{
	if (DailyQuestionary.InspectionData.IsValidIndex(CurrentDayReach))
    {
        CurrentAnomalyDetails = DailyQuestionary.InspectionData.IsValidIndex(CurrentDayReach) ? DailyQuestionary.InspectionData[CurrentDayReach]++ : nullptr;
    }
}

ATablet* APlayerDesk::GetTabletInstance()
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

void APlayerDesk::DeliverAnomalySheet()
{
    if(GetTabletInstance() )
    {
        UE_LOG(LogTemp, Log, TEXT("Removed anomaly sheet from the tablet."));
        GetTabletInstance()->RemoveSheet();
        OnSurvayCompleted.Broadcast();
    }
}

void APlayerDesk::OnPrintDetailsButtonPressed()
{
  if(GetTabletInstance()->HasSheet())
    {
        UE_LOG(LogTemp, Log, TEXT("Tablet has a sheet."));
        if(GetTabletInstance()->CanBeDelivered())
        {
            UE_LOG(LogTemp, Log, TEXT("Delivering anomaly sheet."));
            DeliverAnomalySheet();
            /// Update button states after delivering the anomaly sheet.
            PrintDetailsButtonComponent->bIsInteractable = false;
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
        PrintDetailsButtonComponent->ChangeInteractablePromptText(FText::FromString("Submit"));
    }
}

// Called every frame
void APlayerDesk::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
