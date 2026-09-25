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
#include "ATool.h"
#include "InspectionProp.h"
#include "Tablet.h"

AHorrorCharacter::AHorrorCharacter()
{
	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->Init(this);

	InventoryComponent = CreateDefaultSubobject<UBPC_Inventory>(TEXT("InventoryComponent"));
}

void AHorrorCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Initialize the walk speed
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

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

			// Disabilita collisione per evitare blocchi o bug col character
			MyTablet->SetActorEnableCollision(false);
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
	}
}

void AHorrorCharacter::DoStartUsingTool()
{
	UE_LOG(LogTemp, Warning, TEXT("DoStartUsingTool called"));

	if (InventoryComponent && InventoryComponent->EquippedTool)
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
	//.. add crouch logic here
	Crouch();
}

void AHorrorCharacter::DoEndCrouch()
{
}

void AHorrorCharacter::EnterInspectionMode(AInspectionProp* PropToInspect, ABaseInteractable* Interactable)
{
	bIsInspecting = true;
	if (!PropToInspect){
		CurrentInteractable = Interactable;
		return;
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

	// Mostra il mouse se necessario o blocca il movimento
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetIgnoreMoveInput(true);
	}
}

void AHorrorCharacter::ExitInspectionMode()
{
	bIsInspecting = false;

	if (!CurrentInspectedProp){
		UE_LOG(LogTemp, Warning, TEXT("No inspected prop to exit from."));
		if (CurrentInteractable)
		{
			UE_LOG(LogTemp, Warning, TEXT("FALLBACK: Ending interaction with interactable."));
			CurrentInteractable->OnEndInteract(); // Call the end interaction logic for the interactable
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

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetIgnoreMoveInput(false);
	}
}

void AHorrorCharacter::DoZoom(const FInputActionValue& Value)
{
	if (bIsInspecting && CurrentInspectedProp)
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

void AHorrorCharacter::DoStartInteract()
{
	if (bIsInspecting) return; // Non interagire con altro mentre ispezioni

	if(InteractionComponent->HasInteractableInRange())
	{
		InteractionComponent->CurrentInteractable->OnInteract();
	}
}

void AHorrorCharacter::DoEndInteract()
{
	// Gestione tasto destro (potrebbe essere UseItem in questo progetto)
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

AATool* AHorrorCharacter::GetEquippedTool()
{
	return InventoryComponent ? InventoryComponent->EquippedTool : nullptr;
}

void AHorrorCharacter::Die()
{
	// Disable movement
	DisableMovement();

	// Trigger the OnPlayerDied event
	OnPlayerDied.Broadcast();
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
	/// Catch the case where the player is inspecting an object and wants to drop the tool
	if (bIsInspecting)
	{
		ExitInspectionMode();
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

void AHorrorCharacter::ToggleTablet()
{
	/// CHECK IF IT WORKS

	// if (bIsInspecting)
	// {
	// 	UE_LOG(LogTemp, Log, TEXT("Cannot toggle tablet while inspecting."));
	// 	return;
	// }
	SetTabletOpen(!bIsTabletOpen);
}

void AHorrorCharacter::SetTabletOpen(bool bOpen)
{
	if (!MyTablet)
	{
		UE_LOG(LogTemp, Warning, TEXT("MyTablet is null. Spawning on demand or aborting."));
		return;
	}

	bIsTabletOpen = bOpen;

	APlayerController* PC = Cast<APlayerController>(GetController());

	if (bIsTabletOpen)
	{
		MyTablet->SetActorHiddenInGame(false);

		// Double-check attachment in case it wasn't successful during BeginPlay
		if (MyTablet->GetRootComponent() && MyTablet->GetRootComponent()->GetAttachParent() == nullptr)
		{
			UE_LOG(LogTemp, Warning, TEXT("Tablet was not attached! Retrying attachment in SetTabletOpen..."));
			MyTablet->GetRootComponent()->SetMobility(EComponentMobility::Movable);
			
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
				ParentComp = GetFirstPersonCameraComponent();
			}

			if (ParentComp)
			{
				FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
				MyTablet->AttachToComponent(ParentComp, AttachRules);
			}
		}

		// Apply relative offset to make sure it's positioned perfectly on the left side of the camera
		MyTablet->SetActorRelativeLocation(TabletSocketOffset);
		MyTablet->SetActorRelativeRotation(TabletSocketRotation);

		if (PC)
		{
			PC->SetIgnoreMoveInput(true);
			PC->SetIgnoreLookInput(true);

			FInputModeGameAndUI InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			
			if (MyTablet->TabletWidgetComponent && MyTablet->TabletWidgetComponent->GetUserWidgetObject())
			{
				InputMode.SetWidgetToFocus(MyTablet->TabletWidgetComponent->GetUserWidgetObject()->TakeWidget());
			}
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = true;
		}
	}
	else
	{
		MyTablet->SetActorHiddenInGame(true);

		if (PC)
		{
			PC->SetIgnoreMoveInput(false);
			PC->SetIgnoreLookInput(false);

			FInputModeGameOnly InputMode;
			PC->SetInputMode(InputMode);
			PC->bShowMouseCursor = false;
		}
	}
}

