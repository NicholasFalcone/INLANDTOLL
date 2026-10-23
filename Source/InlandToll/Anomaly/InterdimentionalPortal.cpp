// Fill out your copyright notice in the Description page of Project Settings.


#include "Anomaly/InterdimentionalPortal.h"
#include "GameFramework/PlayerController.h"
#include "InlandTollPlayerController.h"

AInterdimentionalPortal::AInterdimentionalPortal()
{
    isUnlocked = false;
 
    LockerCode = CreateDefaultSubobject<UWidgetComponent>(TEXT("LockerCode"));
    LockerCode->SetupAttachment(RootComponent);

    PortalCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PortalCamera"));
    PortalCamera->SetupAttachment(RootComponent);
}

void AInterdimentionalPortal::SetPlayerCameraToLocker()
{
    AInlandTollPlayerController* PlayerController = Cast<AInlandTollPlayerController>(GetWorld()->GetFirstPlayerController());
    if (!PlayerController || !PlayerController->RequestInteractionState(EPlayerState::Interacting, LockerCode ? LockerCode->GetUserWidgetObject() : nullptr))
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot open the locker interface in the current player state."));
        return;
    }

    PlayerController->SetViewTargetWithBlend(this);
    LockerCode->SetVisibility(true);
    if (!bExitInputBound && PlayerController->InputComponent)
    {
        PlayerController->InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AInterdimentionalPortal::ExitPlayerCameraFromLocker);
        bExitInputBound = true;
    }
}

void AInterdimentionalPortal::CheckUnlockCode(FString EnteredCode)
{
    if (EnteredCode == UnlockCode)
    {
        UnlockPortal();
    }
}

void AInterdimentionalPortal::UnlockPortal(){
    ExitPlayerCameraFromLocker();
    isUnlocked = true;
}

void AInterdimentionalPortal::ExitPlayerCameraFromLocker()
{
    AInlandTollPlayerController* PlayerController = Cast<AInlandTollPlayerController>(GetWorld()->GetFirstPlayerController());
    if (!PlayerController || PlayerController->GetInteractionState() != EPlayerState::Interacting)
    {
        return;
    }

    LockerCode->SetVisibility(false);
    PlayerController->SetViewTargetWithBlend(PlayerController->GetPawn());
    PlayerController->RequestInteractionState(EPlayerState::Unconstrained);
}
