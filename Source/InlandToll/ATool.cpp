// Fill out your copyright notice in the Description page of Project Settings.

#include "ATool.h"
#include "InlandTollCharacter.h"
#include "Variant_Horror/HorrorCharacter.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Camera/CameraComponent.h"
#include "Engine/World.h"

// Sets default values
AATool::AATool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SocketOffset = FVector::ZeroVector;
	SocketRotation = FRotator::ZeroRotator;
	AttachSocketName = NAME_None;
	bIsEquipped = false;
}

// Called when the game starts or when spawned
void AATool::BeginPlay()
{
	Super::BeginPlay();
	
	if (InteractionPromptData.InteractionPrompt.IsEmpty())
	{
		InteractionPromptData.InteractionPrompt = FText::FromString(TEXT("Raccogli Strumento"));
	}
}

// Called every frame
void AATool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AATool::OnEquipped(ACharacter* InOwnerCharacter)
{
	bIsEquipped = true;
	SetActorHiddenInGame(false);
	
	if (MeshComp)
	{
		MeshComp->SetSimulatePhysics(false);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MeshComp->SetVisibility(true, true);
	}

	OnUnhighlight(); // Nasconde l'UI di interazione quando equipaggiato

	if (InOwnerCharacter)
	{
		SetOwner(InOwnerCharacter);

		if (AInlandTollCharacter* TollChar = Cast<AInlandTollCharacter>(InOwnerCharacter))
		{
			USceneComponent* ParentComp = nullptr;
			if (!AttachSocketName.IsNone() && TollChar->GetFirstPersonMesh())
			{
				ParentComp = TollChar->GetFirstPersonMesh();
			}
			else
			{
				ParentComp = TollChar->GetFirstPersonCameraComponent();
			}

			if (ParentComp)
			{
				FAttachmentTransformRules AttachRules(EAttachmentRule::SnapToTarget, EAttachmentRule::SnapToTarget, EAttachmentRule::KeepWorld, false);
				AttachToComponent(ParentComp, AttachRules, AttachSocketName);
				
				SetActorRelativeLocation(SocketOffset);
				SetActorRelativeRotation(SocketRotation);
			}
		}
	}
}

void AATool::OnUnequipped()
{
	bIsEquipped = false;
	SetActorHiddenInGame(true);
	if (MeshComp)
	{
		MeshComp->SetVisibility(false, true);
	}

	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
}

void AATool::OnDropped(ACharacter* InDroppedByCharacter)
{
	bIsEquipped = false;
	SetOwner(nullptr);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorHiddenInGame(false);

	if (InDroppedByCharacter)
	{
		FVector DropLocation = InDroppedByCharacter->GetActorLocation() + (InDroppedByCharacter->GetActorForwardVector() * 80.00f);
		DropLocation.Z += 10.0f; // Un po' sollevato da terra
		
		SetActorLocationAndRotation(DropLocation, InDroppedByCharacter->GetActorRotation());
	}

	if (MeshComp)
	{
		MeshComp->SetVisibility(true, true);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetSimulatePhysics(true);
	}
}

void AATool::OnInteract()
{
	Super::OnInteract();

	if (bIsEquipped) return;

	APlayerController* PC = GetWorld() ? GetWorld()->GetFirstPlayerController() : nullptr;
	if (PC && PC->GetCharacter())
	{
		AHorrorCharacter* HorrorChar = Cast<AHorrorCharacter>(PC->GetCharacter());
		if (HorrorChar)
		{
			HorrorChar->EquipToolFromGround(this);
		}
	}
}

void AATool::OnUsed()
{
	// Implement logic for when the tool is used
}

