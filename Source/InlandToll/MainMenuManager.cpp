// Fill out your copyright notice in the Description page of Project Settings.


#include "MainMenuManager.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"

// Sets default values
AMainMenuManager::AMainMenuManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMainMenuManager::BeginPlay()
{
	Super::BeginPlay();

	ShowMainMenu(); // Show the main menu when the game starts
}

void AMainMenuManager::ShowMainMenu()
{
   if (!MenuWidgetClass || CurrentMenuInstance) return;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (!PC) return;

    CurrentMenuInstance = CreateWidget<UUserWidget>(PC, MenuWidgetClass);
    if (CurrentMenuInstance)
    {
        CurrentMenuInstance->AddToViewport(10); // Z-Order alto per stare sopra il resto

        // Imposta l'input mode per la UI
        FInputModeUIOnly InputMode;
        InputMode.SetWidgetToFocus(CurrentMenuInstance->TakeWidget());
        InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
        
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = true;
    }
}

void AMainMenuManager::HideMainMenu()
{
	if (!CurrentMenuInstance) return;

    CurrentMenuInstance->RemoveFromParent();
    CurrentMenuInstance = nullptr;

    APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PC)
    {
        FInputModeGameOnly InputMode;
        PC->SetInputMode(InputMode);
        PC->bShowMouseCursor = false;
    }
}


