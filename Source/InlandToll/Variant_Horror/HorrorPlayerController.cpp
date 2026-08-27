// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Horror/HorrorPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "InlandTollCameraManager.h"
#include "HorrorCharacter.h"
#include "UI_Master.h"
#include "DialogueBox.h"
#include "InlandToll.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "Kismet/GameplayStatics.h"
#include "InspectionCarManager.h"

AHorrorPlayerController::AHorrorPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AInlandTollCameraManager::StaticClass();
}

void AHorrorPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (ShouldUseTouchControls() && IsLocalPlayerController())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogInlandToll, Error, TEXT("Could not spawn mobile controls widget."));

		}
	}
}

void AHorrorPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	// only spawn UI on local player controllers
	if (IsLocalPlayerController())
	{
		// set up the UI for the character
		if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(aPawn))
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

				if (AInspectionCarManager* CarManager = Cast<AInspectionCarManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AInspectionCarManager::StaticClass())))
				{
					MainUI->SetupManager(CarManager);
				}
			}
		}
	}
	
}

void AHorrorPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	
	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}	
}

bool AHorrorPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
