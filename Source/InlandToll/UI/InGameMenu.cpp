#include "InGameMenu.h"
#include "InspectionManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

void UInGameMenu::NativeConstruct()
{
    Super::NativeConstruct();
    InspectionManager = Cast<AInspectionManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AInspectionManager::StaticClass()));
}

void UInGameMenu::OpenPauseMenu()
{
    SetVisibility(ESlateVisibility::Visible);
    
    // 1. Mette in pausa la simulazione del gioco (fisica, tick, ecc.)
    UGameplayStatics::SetGamePaused(GetWorld(), true);

    // 2. Ottiene il Player Controller
    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    }

    if (PC)
    {
        // 3. Mostra il cursore del mouse a schermo
        PC->bShowMouseCursor = true;

        // 4. Imposta l'input mode per permettere l'interazione con l'interfaccia (UI) e blocca il movimento
        FInputModeGameAndUI InputModeData;
        InputModeData.SetWidgetToFocus(TakeWidget());
        InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        
        PC->SetInputMode(InputModeData);
    }
}

void UInGameMenu::ClosePauseMenu()
{
    SetVisibility(ESlateVisibility::Collapsed);
    UGameplayStatics::SetGamePaused(GetWorld(), false);

    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    }

    if (PC)
    {
        PC->bShowMouseCursor = false;

        FInputModeGameOnly InputModeData;
        PC->SetInputMode(InputModeData);
    }
}

void UInGameMenu::RestartGame()
{
    if (InspectionManager)
    {
        UE_LOG(LogTemp, Warning, TEXT("Restarting game..."));
        InspectionManager->RestartGame();
    }
}

void UInGameMenu::BackToGame()
{
    ClosePauseMenu();
}

void UInGameMenu::BackToMainMenu()
{
    // Disattiva la pausa prima di viaggiare, per evitare problemi nel caricamento
    UGameplayStatics::SetGamePaused(GetWorld(), false);

    APlayerController* PC = GetOwningPlayer();
    if (!PC)
    {
        PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    }

    if (PC)
    {
        PC->bShowMouseCursor = false;

        FInputModeGameOnly InputModeData;
        PC->SetInputMode(InputModeData);
    }

    // Nome del livello esatto come indicato nel Content Browser
    FName MapName = FName(TEXT("MainMenu"));

    // Cambia il livello
    UGameplayStatics::OpenLevel(this, MapName);
}

