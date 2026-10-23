#include "InGameMenu.h"
#include "Desk.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "InlandTollPlayerController.h"

void UInGameMenu::NativeConstruct()
{
    Super::NativeConstruct();
    DeskInstance = Cast<ADesk>(UGameplayStatics::GetActorOfClass(GetWorld(), ADesk::StaticClass()));
}

void UInGameMenu::OpenPauseMenu()
{
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    }

    AInlandTollPlayerController* InlandTollPC = Cast<AInlandTollPlayerController>(PC);
    if (!InlandTollPC || !InlandTollPC->RequestInteractionState(EPlayerState::Paused, this))
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot open pause menu because the player state controller is unavailable."));
        return;
    }

    SetVisibility(ESlateVisibility::Visible);
}

void UInGameMenu::ClosePauseMenu()
{
    SetVisibility(ESlateVisibility::Collapsed);

    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    }

    if (AInlandTollPlayerController* InlandTollPC = Cast<AInlandTollPlayerController>(PC))
    {
        InlandTollPC->ResumeFromPause();
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot close pause menu because the player state controller is unavailable."));
    }
}

void UInGameMenu::RestartGame()
{
    if (DeskInstance)
    {
        UE_LOG(LogTemp, Warning, TEXT("Restarting game..."));
        DeskInstance->RestartGame();
    }
}

void UInGameMenu::BackToGame()
{
    ClosePauseMenu();
}

void UInGameMenu::BackToMainMenu()
{
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    }

    if (AInlandTollPlayerController* InlandTollPC = Cast<AInlandTollPlayerController>(PC))
    {
        InlandTollPC->RequestInteractionState(EPlayerState::MainMenu);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Cannot update player state before returning to the main menu."));
        UGameplayStatics::SetGamePaused(GetWorld(), false);
    }

    // Nome del livello esatto come indicato nel Content Browser
    FName MapName = FName(TEXT("MainMenu"));

    // Cambia il livello
    UGameplayStatics::OpenLevel(this, MapName);
}
