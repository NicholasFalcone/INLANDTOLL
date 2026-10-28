// Copyright Epic Games, Inc. All Rights Reserved.


#include "InlandTollPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "InlandTollCameraManager.h"
#include "Blueprint/UserWidget.h"
#include "InlandToll.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "Kismet/GameplayStatics.h"
#include "Variant_Horror/HorrorCharacter.h"

AInlandTollPlayerController::AInlandTollPlayerController()
{
	// set the player camera manager class
	PlayerCameraManagerClass = AInlandTollCameraManager::StaticClass();
}

void AInlandTollPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ApplyPlayerState(CurrentPlayerState, nullptr);
	
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

void AInlandTollPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ApplyPlayerState(CurrentPlayerState, nullptr);
}

bool AInlandTollPlayerController::RequestInteractionState(EPlayerState NewState, UUserWidget* WidgetToFocus)
{
	if (CurrentPlayerState == EPlayerState::Dead && NewState != EPlayerState::Dead)
	{
		return false;
	}

	if (CurrentPlayerState == EPlayerState::Paused)
	{
		if (NewState == EPlayerState::Paused)
		{
			ApplyPlayerState(NewState, WidgetToFocus);
			return true;
		}

		if (NewState != EPlayerState::MainMenu && NewState != EPlayerState::Dead)
		{
			return false;
		}
	}
	else if (NewState == EPlayerState::Paused && (CurrentPlayerState == EPlayerState::MainMenu || CurrentPlayerState == EPlayerState::Dead))
	{
		return false;
	}

	if (NewState == CurrentPlayerState)
	{
		ApplyPlayerState(NewState, WidgetToFocus);
		return true;
	}

	if (NewState == EPlayerState::Paused)
	{
		PreviousInteractionStates.Add(CurrentPlayerState);
	}
	else if (NewState == EPlayerState::Tablet)
	{
		if (CurrentPlayerState != EPlayerState::Unconstrained && CurrentPlayerState != EPlayerState::Inspecting)
		{
			return false;
		}
		PreviousInteractionStates.Add(CurrentPlayerState);
	}
	else if (NewState == EPlayerState::Inspecting || NewState == EPlayerState::Interacting)
	{
		if (CurrentPlayerState != EPlayerState::Unconstrained)
		{
			return false;
		}
		PreviousInteractionStates.Add(CurrentPlayerState);
	}
	else if (NewState == EPlayerState::MainMenu)
	{
		if (CurrentPlayerState != EPlayerState::Unconstrained && CurrentPlayerState != EPlayerState::Paused)
		{
			return false;
		}
		PreviousInteractionStates.Reset();
	}
	else if (NewState == EPlayerState::Unconstrained || NewState == EPlayerState::Dead)
	{
		PreviousInteractionStates.Reset();
	}

	CurrentPlayerState = NewState;
	ApplyPlayerState(NewState, WidgetToFocus);
	return true;
}

void AInlandTollPlayerController::ResumeFromPause()
{
	if (CurrentPlayerState == EPlayerState::Paused)
	{
		PopInteractionState(EPlayerState::Paused);
	}
}

bool AInlandTollPlayerController::PopInteractionState(EPlayerState ExpectedState)
{
	if (CurrentPlayerState != ExpectedState || PreviousInteractionStates.IsEmpty())
	{
		return false;
	}

	CurrentPlayerState = PreviousInteractionStates.Pop(EAllowShrinking::No);
	ApplyPlayerState(CurrentPlayerState, nullptr);
	return true;
}

void AInlandTollPlayerController::EndInteractionState(EPlayerState State)
{
	if (CurrentPlayerState == State)
	{
		PopInteractionState(State);
		return;
	}

	const int32 StateIndex = PreviousInteractionStates.FindLast(State);
	if (StateIndex != INDEX_NONE)
	{
		PreviousInteractionStates.RemoveAt(StateIndex);
		if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(GetPawn()))
		{
			HorrorCharacter->ApplyInteractionState(CurrentPlayerState);
		}
	}
}

bool AInlandTollPlayerController::HasInteractionState(EPlayerState State) const
{
	return CurrentPlayerState == State || PreviousInteractionStates.Contains(State);
}

void AInlandTollPlayerController::ResetInteractionState()
{
	PreviousInteractionStates.Reset();
	CurrentPlayerState = EPlayerState::Unconstrained;
	ApplyPlayerState(CurrentPlayerState, nullptr);
}

void AInlandTollPlayerController::ApplyPlayerState(EPlayerState NewState, UUserWidget* WidgetToFocus)
{
	const bool bPaused = NewState == EPlayerState::Paused;
	if (GetWorld())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), bPaused);
	}

	if (bPaused)
	{
		ApplyInputBlock(true, true);
		if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(GetPawn()))
		{
			HorrorCharacter->ApplyInteractionState(NewState);
			HorrorCharacter->SetTabletInteractionEnabled(false);
		}

		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		if (WidgetToFocus)
		{
			InputMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
		}
		SetInputMode(InputMode);
		bShowMouseCursor = true;
		return;
	}

	if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(GetPawn()))
	{
		HorrorCharacter->ApplyInteractionState(NewState);
	}

	const bool bIsTablet = NewState == EPlayerState::Tablet;
	const bool bIsInspecting = NewState == EPlayerState::Inspecting;
	const bool bIsInteracting = NewState == EPlayerState::Interacting;
	const bool bIsMenu = NewState == EPlayerState::MainMenu;
	ApplyInputBlock(bIsTablet || bIsInspecting || bIsInteracting || bIsMenu || NewState == EPlayerState::Dead,
		bIsTablet || bIsInteracting || bIsMenu || NewState == EPlayerState::Dead);

	if (bIsTablet || bIsInteracting)
	{
		FInputModeGameAndUI InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		InputMode.SetHideCursorDuringCapture(false);
		if (WidgetToFocus)
		{
			InputMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
		}
		SetInputMode(InputMode);
		if (bIsTablet)
		{
			if (AHorrorCharacter* HorrorCharacter = Cast<AHorrorCharacter>(GetPawn()))
			{
				HorrorCharacter->FocusTabletWidget();
			}
		}
	}
	else if (bIsMenu)
	{
		FInputModeUIOnly InputMode;
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		if (WidgetToFocus)
		{
			InputMode.SetWidgetToFocus(WidgetToFocus->TakeWidget());
		}
		SetInputMode(InputMode);
	}
	else
	{
		FInputModeGameOnly InputMode;
		SetInputMode(InputMode);
	}

	bShowMouseCursor = bIsTablet || bIsInteracting || bIsMenu;
}

void AInlandTollPlayerController::ApplyInputBlock(bool bBlockMovement, bool bBlockLook)
{
	if (bMovementInputBlocked != bBlockMovement)
	{
		SetIgnoreMoveInput(bBlockMovement);
		bMovementInputBlocked = bBlockMovement;
	}

	if (bLookInputBlocked != bBlockLook)
	{
		SetIgnoreLookInput(bBlockLook);
		bLookInputBlocked = bBlockLook;
	}
}

void AInlandTollPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Context
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

bool AInlandTollPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}
