// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Horror/HorrorCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "InputActionValue.h"
#include "InteractionComponent.h"
#include "InputAction.h"
#include "HorrorCharacter.h"
#include "Tools/Tool.h"
#include "InspectionProp.h"
#include "Tools/Tablet.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/WidgetComponent.h"
#include "Framework/Application/SlateApplication.h"
#include "InlandTollPlayerController.h"

AHorrorCharacter::AHorrorCharacter()
{
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->Init(this);
	InventoryComponent = CreateDefaultSubobject<UBPC_Inventory>(TEXT("InventoryComponent"));

	TabletWidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("TabletWidgetInteraction"));
	TabletWidgetInteraction->SetupAttachment(GetRootComponent());
	TabletWidgetInteraction->InteractionSource = EWidgetInteractionSource::Mouse;
	TabletWidgetInteraction->InteractionDistance = 500.f;
	TabletWidgetInteraction->TraceChannel = ECC_Visibility;
	TabletWidgetInteraction->bEnableHitTesting = false;
	TabletWidgetInteraction->PointerIndex = 0;
}

void AHorrorCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the walk speed
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MaxWalkSpeedCrouched = CrouchWalkSpeed;
	
	if (TabletClass)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		MyTablet = GetWorld()->SpawnActor<ATablet>(TabletClass, SpawnParams);
		if (MyTablet)
		{
			if (MyTablet->GetRootComponent())
			{
				MyTablet->GetRootComponent()->SetMobility(EComponentMobility::Movable);
			}

			USceneComponent* ParentComp = nullptr;
			TArray<USpringArmComponent*> SpringArms;
			GetComponents<USpringArmComponent>(SpringArms);
			for (USpringArmComponent* Arm : SpringArms)
			{
				if (Arm && !Arm->IsTemplate() && Arm->GetName().Contains(TEXT("Tablet")))
				{
					ParentComp = Arm;
					break;
				}
			}
			if (!ParentComp)
			{
				USceneComponent* RawComp = GetTabletSpringArmComponent();
				if (RawComp && !RawComp->IsTemplate())
				{
					ParentComp = RawComp;
				}
			}
			if (!ParentComp)
			{
				UE_LOG(LogTemp, Warning, TEXT("GetTabletSpringArmComponent returned a template or null, falling back to FirstPersonCameraComponent"));
				ParentComp = GetFirstPersonCameraComponent();
			}

			if (ParentComp)
			{
				UE_LOG(LogTemp, Warning, TEXT("Attaching tablet to parent component: %s"), *ParentComp->GetName());
				FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
				bool bAttached = MyTablet->AttachToComponent(ParentComp, AttachRules);
				if (bAttached)
				{
					UE_LOG(LogTemp, Log, TEXT("Tablet successfully attached to %s"), *ParentComp->GetName());
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Tablet attachment returned false!"));
				}
			}
			else
			{
				UE_LOG(LogTemp, Error, TEXT("No valid parent component found for Tablet attachment!"));
			}
			
			MyTablet->SetActorRelativeLocation(TabletSocketOffset);
			MyTablet->SetActorRelativeRotation(TabletSocketRotation);

			// Disabilita collisione per componente per evitare blocchi col character.
			// SetActorEnableCollision(false) disattiverebbe anche le query del widget, quindi resta true.
			MyTablet->SetActorEnableCollision(true);
			TArray<UPrimitiveComponent*> PrimComps;
			MyTablet->GetComponents<UPrimitiveComponent>(PrimComps);
			for (UPrimitiveComponent* PrimComp : PrimComps)
			{
				PrimComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
				PrimComp->SetCollisionResponseToAllChannels(ECR_Ignore);
			}

			// Inizialmente nascosto
			MyTablet->SetActorHiddenInGame(true);
			bIsTabletOpen = false;
		}
	}
}

void AHorrorCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

}

void AHorrorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	{
		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
		{
			// Using item
			EnhancedInputComponent->BindAction(UseItem, ETriggerEvent::Started, this, &AHorrorCharacter::DoStartUsingTool);
			EnhancedInputComponent->BindAction(UseItem, ETriggerEvent::Completed, this, &AHorrorCharacter::DoEndUsingTool);
			// Interact with item
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoStartInteract);
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AHorrorCharacter::DoEndInteract);

			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoStartCrouch);
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AHorrorCharacter::DoEndCrouch);

			// Drop Tool (Usually bound to Q key)
			if (DropAction)
			{
				EnhancedInputComponent->BindAction(DropAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoDropTool);
			}

			// Zoom Input (Mouse Scroll Wheel during inspection)
			if (ZoomAction)
			{
				EnhancedInputComponent->BindAction(ZoomAction, ETriggerEvent::Triggered, this, &AHorrorCharacter::DoZoom);
			}

			// Toggle Tablet Input (TAB Key behavior)
			if (ToggleTabletAction)
			{
				EnhancedInputComponent->BindAction(ToggleTabletAction, ETriggerEvent::Started, this, &AHorrorCharacter::ToggleTablet);
			}
		}

		// Inoltra il click sinistro al widget 3D del tablet
		PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Pressed, this, &AHorrorCharacter::TabletMousePressed);
		PlayerInputComponent->BindKey(EKeys::LeftMouseButton, IE_Released, this, &AHorrorCharacter::TabletMouseReleased);
	}
}

void AHorrorCharacter::TabletMousePressed()
{
	if (bIsTabletOpen && TabletWidgetInteraction && TabletWidgetInteraction->bEnableHitTesting)
	{
		TabletWidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
	}
}

void AHorrorCharacter::TabletMouseReleased()
{
	if (bIsTabletOpen && TabletWidgetInteraction && TabletWidgetInteraction->bEnableHitTesting)
	{
		TabletWidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
	}
}

#pragma region InputActions

void AHorrorCharacter::DoStartUsingTool()
{
	UE_LOG(LogTemp, Warning, TEXT("DoStartUsingTool called"));

	if (CurrentPlayerState == EPlayerState::Unconstrained && InventoryComponent && InventoryComponent->EquippedTool)
	{
		InventoryComponent->EquippedTool->OnUsed();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No tool equipped to use."));
	}
}

void AHorrorCharacter::DoEndUsingTool()
{
	//..
}

void AHorrorCharacter::DoStartCrouch()
{
	if(CurrentPlayerState == EPlayerState::Unconstrained)
	{
		UE_LOG(LogTemp, Warning, TEXT("DoStartCrouch called"));
		//.. add crouch logic here
		Crouch();
	}
}

void AHorrorCharacter::DoEndCrouch()
{
	UE_LOG(LogTemp, Warning, TEXT("DoEndCrouch called"));
	//.. add uncrouch logic here
	UnCrouch();
}

void AHorrorCharacter::DoStartInteract()
{
	if (CurrentPlayerState != EPlayerState::Unconstrained || !InteractionComponent) return;

	if(InteractionComponent->HasInteractableInRange())
	{
		InteractionComponent->CurrentInteractable->OnInteract();
	}
}

void AHorrorCharacter::DoJumpStart()
{
	if (CurrentPlayerState == EPlayerState::Unconstrained)
	{
		Super::DoJumpStart();
	}
}

void AHorrorCharacter::DoEndInteract()
{
	// Gestione tasto destro (potrebbe essere UseItem in questo progetto)
}

void AHorrorCharacter::DoZoom(const FInputActionValue& Value)
{
	if (CurrentPlayerState != EPlayerState::Paused && bIsInspecting && CurrentInspectedProp)
	{
		float AxisValue = Value.Get<float>();

		float Speed = CurrentInspectedProp->ZoomSpeed;
		float MinOffset = CurrentInspectedProp->MinInspectionOffset;
		float MaxOffset = CurrentInspectedProp->MaxInspectionOffset;

		// Scroll up (positive AxisValue) decreases the offset to zoom in (brings object closer)
		CurrentInspectionOffset -= AxisValue * Speed;
		CurrentInspectionOffset = FMath::Clamp(CurrentInspectionOffset, MinOffset, MaxOffset);

		if (USceneComponent* RootComp = CurrentInspectedProp->GetRootComponent())
		{
			RootComp->SetRelativeLocation(FVector(CurrentInspectionOffset, 0.f, 0.f));
		}
	}
}

void AHorrorCharacter::DoAim(float Yaw, float Pitch)
{
	if (CurrentPlayerState == EPlayerState::Paused)
	{
		return;
	}

	if (bIsInspecting && CurrentInspectedProp)
	{
		// Accumuliamo Yaw e Pitch in modo che la rotazione sia coerente con la visuale della camera (screen space)
		// e non si perda l'orientamento con l'accumulo delle rotazioni locali.
		TargetInspectionYaw -= Yaw;
		TargetInspectionPitch += Pitch;

		// Clamp della rotazione Pitch per prevenire capovolgimenti indesiderati e gimbal lock
		TargetInspectionPitch = FMath::Clamp(TargetInspectionPitch, -89.0f, 89.0f);

		FRotator NewRelativeRotation = FRotator(TargetInspectionPitch, TargetInspectionYaw, 0.0f);
		if (USceneComponent* RootComp = CurrentInspectedProp->GetRootComponent())
		{
			RootComp->SetRelativeRotation(NewRelativeRotation);
		}
	}
	else
	{
		Super::DoAim(Yaw, Pitch);
	}
}

void AHorrorCharacter::DoMove(float Right, float Forward)
{
	if (!bIsInspecting)
	{
		Super::DoMove(Right, Forward);
	}
}

#pragma endregion InputActions

void AHorrorCharacter::EnterInspectionMode(AInspectionProp* PropToInspect, ABaseInteractable* Interactable)
{
	if (!PropToInspect)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot enter inspection mode without a prop."));
		return;
	}

	AInlandTollPlayerController* PlayerController = Cast<AInlandTollPlayerController>(GetController());
	if (!PlayerController || !PlayerController->RequestInteractionState(EPlayerState::Inspecting))
	{
		return;
	}

	if (!PropToInspect->GetRootComponent())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot inspect prop without a root component."));
		PlayerController->RequestInteractionState(EPlayerState::Unconstrained);
		return;
	}

	if (CurrentInspectedProp)
	{
		DropProp();
	}

	if (Interactable)
	{
		CurrentInteractable = Interactable;
	}

	CurrentInspectedProp = PropToInspect;

	// Salva lo stato di attachment originale prima di cambiare parent
	CurrentInspectedProp->OriginalParentComponent = CurrentInspectedProp->GetRootComponent()->GetAttachParent();
	CurrentInspectedProp->OriginalSocketName = CurrentInspectedProp->GetRootComponent()->GetAttachSocketName();

	// Salva la posizione/rotazione (relativa se attaccato, world se libero)
	CurrentInspectedProp->OriginalLocation = CurrentInspectedProp->GetRootComponent()->GetRelativeLocation();
	CurrentInspectedProp->OriginalRotation = CurrentInspectedProp->GetRootComponent()->GetRelativeRotation();

	// Prepara l'oggetto per l'ispezione
	CurrentInspectedProp->SetIsInspecting(true);

	// Inizializza zoom e rotazione di ispezione
	CurrentInspectionOffset = CurrentInspectedProp->DefaultInspectionOffset;
	TargetInspectionYaw = 0.0f;
	TargetInspectionPitch = 0.0f;

	// Attacca l'oggetto alla camera
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	CurrentInspectedProp->AttachToComponent(GetFirstPersonCameraComponent(), AttachmentRules);

	// Posizionalo davanti alla camera (usa RootComponent per relative transform su Actor)
	if (USceneComponent* RootComp = CurrentInspectedProp->GetRootComponent())
	{
		RootComp->SetRelativeLocation(FVector(CurrentInspectionOffset, 0.f, 0.f));
		RootComp->SetRelativeRotation(FRotator::ZeroRotator);
	}

}

void AHorrorCharacter::GettingCut()
{
	UE_LOG(LogTemp, Warning, TEXT("Player is getting cut!"));
	OnPlayerGettingCut.Broadcast();
	ExitInspectionMode();
	/// Need to set a timer here to enable input after a delay to simulate recovery from getting cut.
	GetWorld()->GetTimerManager().SetTimer(UnusedHandle, this, &AHorrorCharacter::Recover, 1.0f, false); // 1.0f is the delay in seconds before re-enabling input, false means it won't loop
}

void AHorrorCharacter::Die()
{
	UE_LOG(LogTemp, Warning, TEXT("Player has died."));
	if(IsAlive)
	{
		IsAlive = false;
		if (AInlandTollPlayerController* PlayerController = Cast<AInlandTollPlayerController>(GetController()))
		{
			PlayerController->RequestInteractionState(EPlayerState::Dead);
		}
		else
		{
			ExitInspectionMode();
		}
		// Disable movement
		DisableMovement();
		// Drop Tool
		DoDropTool();
		// Trigger the OnPlayerDied event
		OnPlayerDied.Broadcast();
	}
}

void AHorrorCharacter::Recover()
{
	UE_LOG(LogTemp, Warning, TEXT("Player is recovering from getting cut."));
}

void AHorrorCharacter::ExitInspectionMode()
{
	if (AInlandTollPlayerController* PlayerController = Cast<AInlandTollPlayerController>(GetController()))
	{
		PlayerController->EndInteractionState(EPlayerState::Inspecting);
	}
	else
	{
		bIsInspecting = false;
		CurrentPlayerState = EPlayerState::Unconstrained;
		DropProp();
	}
}

void AHorrorCharacter::DropProp()
{
	if (!CurrentInspectedProp){
		UE_LOG(LogTemp, Warning, TEXT("No inspected prop to exit from."));
		if (CurrentInteractable)
		{
			UE_LOG(LogTemp, Warning, TEXT("FALLBACK: Ending interaction with interactable."));
			CurrentInteractable->OnEndInteract(); // Call the end interaction logic for the interactable
			CurrentInteractable = nullptr;
		}
		return;
	}

	// Se l'oggetto era originariamente attaccato a qualcosa, ripristina l'attachment
	if (CurrentInspectedProp->OriginalParentComponent)
	{
		FAttachmentTransformRules ReattachRules(EAttachmentRule::KeepRelative, false);
		CurrentInspectedProp->AttachToComponent(CurrentInspectedProp->OriginalParentComponent, ReattachRules, CurrentInspectedProp->OriginalSocketName);
		
		if (USceneComponent* RootComp = CurrentInspectedProp->GetRootComponent())
		{
			RootComp->SetRelativeLocation(CurrentInspectedProp->OriginalLocation);
			RootComp->SetRelativeRotation(CurrentInspectedProp->OriginalRotation);
		}
	}
	else
	{
		// Se era libero nel mondo
		CurrentInspectedProp->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		CurrentInspectedProp->SetActorLocation(CurrentInspectedProp->OriginalLocation);
		CurrentInspectedProp->SetActorRotation(CurrentInspectedProp->OriginalRotation);
	}

	CurrentInspectedProp->OnEndInteract(); // Call the end interaction logic for the prop

	CurrentInspectedProp = nullptr;
	CurrentInteractable = nullptr;

}

void AHorrorCharacter::DisableMovement()
{
	GetCharacterMovement()->DisableMovement();
}

void AHorrorCharacter::EnableMovement()
{
	GetCharacterMovement()->SetMovementMode(MOVE_Walking);
}

void AHorrorCharacter::HidePlayerMesh()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetVisibility(false, true); // Set bPropagateToChildren to true to hide all child components as well
	}
}

void AHorrorCharacter::ShowPlayerMesh()
{
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetVisibility(true, true); // Set bPropagateToChildren to true to hide all child components as well
	}
}

#pragma region Tool Functions

AATool* AHorrorCharacter::GetEquippedTool()
{
	return InventoryComponent ? InventoryComponent->EquippedTool : nullptr;
}

void AHorrorCharacter::EquipToolFromGround(AATool* NewTool)
{
	if (!NewTool) return;

	// Se abbiamo già un oggetto in mano, lo lasciamo cadere prima di raccogliere il nuovo
	if (InventoryComponent && InventoryComponent->EquippedTool)
	{
		DoDropTool();
	}

	if (InventoryComponent)
	{
		InventoryComponent->EquipTool(NewTool);
		UE_LOG(LogTemp, Warning, TEXT("Equipped Tool: %s"), *NewTool->GetName());
	}
}

void AHorrorCharacter::DoDropTool()
{
	if (CurrentPlayerState == EPlayerState::Paused)
	{
		return;
	}

	/// Catch the case where the player is inspecting an object and wants to drop the tool
	if (bIsInspecting)
	{
		ExitInspectionMode();
		return;
	}

	if (CurrentPlayerState != EPlayerState::Unconstrained)
	{
		return;
	}

	if (!InventoryComponent) return;

	AATool* ToolToDrop = InventoryComponent->EquippedTool;
	if (ToolToDrop)
	{
		InventoryComponent->EquipTool(nullptr);
		ToolToDrop->OnDropped(this);
		UE_LOG(LogTemp, Warning, TEXT("Dropped Tool: %s"), *ToolToDrop->GetName());
	}
}
#pragma endregion

#pragma region Tablet Functions
void AHorrorCharacter::ToggleTablet()
{
	SetTabletOpen(!bIsTabletOpen);
}

void AHorrorCharacter::SetTabletOpen(bool bOpen)
{
    if (!MyTablet)
    {
        UE_LOG(LogTemp, Warning, TEXT("Cannot change tablet state because MyTablet is null."));
        return;
    }

	AInlandTollPlayerController* PlayerController = Cast<AInlandTollPlayerController>(GetController());
	if (!PlayerController)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot change tablet state because the player controller does not manage player states."));
		return;
	}

	if (bOpen == bIsTabletOpen)
	{
		return;
	}

	if (bOpen)
	{
		PlayerController->RequestInteractionState(EPlayerState::Tablet);
	}
	else
	{
		PlayerController->EndInteractionState(EPlayerState::Tablet);
	}
}

void AHorrorCharacter::ApplyInteractionState(EPlayerState NewState)
{
	AInlandTollPlayerController* PlayerController = Cast<AInlandTollPlayerController>(GetController());
	const bool bShouldBeInspecting = NewState == EPlayerState::Inspecting
		|| (PlayerController && PlayerController->HasInteractionState(EPlayerState::Inspecting));
	const bool bShouldHaveTabletOpen = NewState == EPlayerState::Tablet
		|| (PlayerController && PlayerController->HasInteractionState(EPlayerState::Tablet));

	if (bIsInspecting && !bShouldBeInspecting && CurrentInspectedProp)
	{
		DropProp();
	}

	CurrentPlayerState = NewState;
	bIsInspecting = bShouldBeInspecting;
	bIsTabletOpen = bShouldHaveTabletOpen;

	if (MyTablet)
	{
		MyTablet->SetActorHiddenInGame(!bIsTabletOpen);
		if (bIsTabletOpen)
		{
			MyTablet->SetActorRelativeLocation(TabletSocketOffset);
			MyTablet->SetActorRelativeRotation(TabletSocketRotation);
			if (MyTablet->TabletWidget)
			{
				MyTablet->TabletWidget->EnableSheet();
			}

		}

		if (MyTablet->TabletWidgetComponent)
		{
			if (bIsTabletOpen)
			{
				MyTablet->TabletWidgetComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
				MyTablet->TabletWidgetComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
				MyTablet->TabletWidgetComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
			}
			else
			{
				MyTablet->TabletWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			}
		}
	}

	SetTabletInteractionEnabled(bIsTabletOpen);
}

void AHorrorCharacter::SetTabletInteractionEnabled(bool bEnabled)
{
	if (TabletWidgetInteraction)
	{
		TabletWidgetInteraction->bEnableHitTesting = bEnabled;
	}
}

void AHorrorCharacter::FocusTabletWidget()
{
	if (MyTablet && MyTablet->TabletWidgetComponent)
	{
		if (UUserWidget* TabletUserWidget = MyTablet->TabletWidgetComponent->GetUserWidgetObject())
		{
			TSharedPtr<SWidget> SlateWidget = TabletUserWidget->GetCachedWidget();
			if (SlateWidget.IsValid())
			{
				FSlateApplication::Get().SetKeyboardFocus(SlateWidget, EFocusCause::SetDirectly);
			}
		}
	}
}
#pragma endregion
