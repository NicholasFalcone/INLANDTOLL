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
#include "InspectionProp.h"

AHorrorCharacter::AHorrorCharacter()
{
	// create the spotlight
	SpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("SpotLight"));
	SpotLight->SetupAttachment(GetFirstPersonCameraComponent());

	SpotLight->SetRelativeLocationAndRotation(FVector(30.0f, 17.5f, -5.0f), FRotator(-18.6f, -1.3f, 5.26f));
	SpotLight->Intensity = 0.5;
	SpotLight->SetIntensityUnits(ELightUnits::Lumens);
	SpotLight->AttenuationRadius = 1050.0f;
	SpotLight->InnerConeAngle = 18.7f;
	SpotLight->OuterConeAngle = 45.24f;

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	InteractionComponent->Init(this);
}

void AHorrorCharacter::BeginPlay()
{
	Super::BeginPlay();

	// initialize sprint meter to max
	SprintMeter = SprintTime;

	// Initialize the walk speed
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	// start the sprint tick timer
	GetWorld()->GetTimerManager().SetTimer(SprintTimer, this, &AHorrorCharacter::SprintFixedTick, SprintFixedTickTime, true);
}

void AHorrorCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// clear the sprint timer
	GetWorld()->GetTimerManager().ClearTimer(SprintTimer);
}

void AHorrorCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	{
		// Set up action bindings
		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
		{
			// Sprinting
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoStartSprint);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AHorrorCharacter::DoEndSprint);
			// Using item
			EnhancedInputComponent->BindAction(UseItem, ETriggerEvent::Started, this, &AHorrorCharacter::DoStartToggleLight);
			EnhancedInputComponent->BindAction(UseItem, ETriggerEvent::Completed, this, &AHorrorCharacter::DoEndToggleLight);
			// Interact with item
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoStartInteract);
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AHorrorCharacter::DoEndInteract);

			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AHorrorCharacter::DoStartCrouch);
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AHorrorCharacter::DoEndCrouch);
		}
	}
}

void AHorrorCharacter::DoStartSprint()
{
	// set the sprinting flag
	bSprinting = true;

	// are we out of recovery mode?
	if (!bRecovering)
	{
		// set the sprint walk speed
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

		// call the sprint state changed delegate
		OnSprintStateChanged.Broadcast(true);
	}

}

void AHorrorCharacter::DoEndSprint()
{
	// set the sprinting flag
	bSprinting = false;

	// are we out of recovery mode?
	if (!bRecovering)
	{
		// set the default walk speed
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

		// call the sprint state changed delegate
		OnSprintStateChanged.Broadcast(false);
	}
}

void AHorrorCharacter::DoStartToggleLight()
{
	if (bIsInspecting)
	{
		ExitInspectionMode();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("DoStartUseItem called"));

	if(SpotLight->bHiddenInGame)
	{
		SpotLight->SetHiddenInGame(false);
	}
	else{
		SpotLight->SetHiddenInGame(true);
	}
}

void AHorrorCharacter::DoEndToggleLight()
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

void AHorrorCharacter::EnterInspectionMode(AInspectionProp* PropToInspect)
{
	if (!PropToInspect) return;

	bIsInspecting = true;
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
	if (!CurrentInspectedProp) return;

	bIsInspecting = false;

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

void AHorrorCharacter::SprintFixedTick()
{
	// are we out of recovery, still have stamina and are moving faster than our walk speed?
	if (bSprinting && !bRecovering && GetVelocity().Length() > WalkSpeed)
	{

		// do we still have meter to burn?
		if (SprintMeter > 0.0f)
		{
			// update the sprint meter
			SprintMeter = FMath::Max(SprintMeter - SprintFixedTickTime, 0.0f);

			// have we run out of stamina?
			if (SprintMeter <= 0.0f)
			{
				// raise the recovering flag
				bRecovering = true;

				// set the recovering walk speed
				GetCharacterMovement()->MaxWalkSpeed = RecoveringWalkSpeed;
			}
		}
		
	} else {

		// recover stamina
		SprintMeter = FMath::Min(SprintMeter + SprintFixedTickTime, SprintTime);

		if (SprintMeter >= SprintTime)
		{
			// lower the recovering flag
			bRecovering = false;

			// set the walk or sprint speed depending on whether the sprint button is down
			GetCharacterMovement()->MaxWalkSpeed = bSprinting ? SprintSpeed : WalkSpeed;

			// update the sprint state depending on whether the button is down or not
			OnSprintStateChanged.Broadcast(bSprinting);
		}

	}

	// broadcast the sprint meter updated delegate
	OnSprintMeterUpdated.Broadcast(SprintMeter / SprintTime);

}
