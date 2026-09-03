// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Horror/HorrorCharacter.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SpotLightComponent.h"
#include "EnhancedInputComponent.h"
#include "InteractionComponent.h"
#include "InputAction.h"
#include "HorrorCharacter.h"
#include "ATool.h"
#include "InspectionProp.h"

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

	// Attacca l'oggetto alla camera
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	CurrentInspectedProp->AttachToComponent(GetFirstPersonCameraComponent(), AttachmentRules);

	// Posizionalo davanti alla camera (usa RootComponent per relative transform su Actor)
	if (USceneComponent* RootComp = CurrentInspectedProp->GetRootComponent())
	{
		RootComp->SetRelativeLocation(FVector(InspectionOffset, 0.f, 0.f));
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

void AHorrorCharacter::DoAim(float Yaw, float Pitch)
{
	if (bIsInspecting && CurrentInspectedProp)
	{
		// Invece di ruotare la camera, ruotiamo l'oggetto
		FRotator NewRotation = FRotator(Pitch, Yaw, 0.f);
		CurrentInspectedProp->AddActorLocalRotation(NewRotation);
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

