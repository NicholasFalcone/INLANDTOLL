// Fill out your copyright notice in the Description page of Project Settings.


#include "Anomaly/InterdimentionalPortal.h"

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
    GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this);
    /// Set player input mode to Gameplay and UI
    FInputModeGameAndUI InputMode;
    GetWorld()->GetFirstPlayerController()->SetInputMode(InputMode);
    GetWorld()->GetFirstPlayerController()->bShowMouseCursor = true;
    LockerCode->SetVisibility(true);
    /// Handle the ESC key to exit the locker camera
    GetWorld()->GetFirstPlayerController()->InputComponent->BindKey(EKeys::RightMouseButton, IE_Pressed, this, &AInterdimentionalPortal::ExitPlayerCameraFromLocker);
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
    LockerCode->SetVisibility(false);
    GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(GetWorld()->GetFirstPlayerController()->GetPawn());
    FInputModeGameOnly InputMode;
    GetWorld()->GetFirstPlayerController()->SetInputMode(InputMode);
    GetWorld()->GetFirstPlayerController()->bShowMouseCursor = false;
}
