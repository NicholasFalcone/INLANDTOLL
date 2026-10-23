// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Horror/HorrorPlayerController.h"
#include "HorrorCharacter.h"
#include "UI/UI_Master.h"
#include "UI/DialogueBox.h"
#include "Kismet/GameplayStatics.h"
#include "Desk.h"
#include "Bed.h"

AHorrorPlayerController::AHorrorPlayerController()
{
}

void AHorrorPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	// only spawn UI on local player controllers
	if (IsLocalPlayerController())
	{
		// set up the UI for the character
		if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(InPawn))
		{
			// create the UI
			if (!MainUI)
			{
				MainUI = CreateWidget<UUI_Master>(this, MainUIClass);
				MainUI->AddToViewport(0);
			}

			// create dialogue UI
			if (!DialogueBoxWidget && DialogueBoxWidgetClass)
			{
				DialogueBoxWidget = CreateWidget<UDialogueBox>(this, DialogueBoxWidgetClass);
				if (DialogueBoxWidget)
				{
					DialogueBoxWidget->AddToViewport(0);
				}
			}

			if (MainUI)
			{
				MainUI->SetupCharacter(HorrorCharacter);

				if (ADesk* DeskInstance = Cast<ADesk>(UGameplayStatics::GetActorOfClass(GetWorld(), ADesk::StaticClass())))
				{
					if (ABed* BedInstance = Cast<ABed>(UGameplayStatics::GetActorOfClass(GetWorld(), ABed::StaticClass())))
					{
						MainUI->SetupManager(DeskInstance, BedInstance);
					}
				}
			}
		}
	}
	
}

void AHorrorPlayerController::OnUnPossess()
{
	Super::OnUnPossess();

	if (MainUI)
    {
        MainUI->RemoveFromParent();
        MainUI = nullptr; // Resettando il puntatore, OnPossess ricreerà e riconnetterà la UI pulita
    }

    if (DialogueBoxWidget)
    {
        DialogueBoxWidget->RemoveFromParent();
        DialogueBoxWidget = nullptr;
    }
}
