// Fill out your copyright notice in the Description page of Project Settings.


#include "Anomaly/LostMemory.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Components/StaticMeshComponent.h"
#include "Camera/CameraComponent.h"

void ALostMemory::BeginPlay()
{
	Super::BeginPlay();

	if (MeshComp)
	{
		PostcardMaterial = MeshComp->CreateDynamicMaterialInstance(MaterialSlotIndex);
	}
	CurrentState = 1;
	ApplyCurrentTexture();
}

void ALostMemory::OnInteract()
{
	Super::OnInteract();
	bShowingBack = false;
}

void ALostMemory::ApplyCurrentTexture()
{
	if (PostcardMaterial && PostcardTextures.IsValidIndex(CurrentState - 1) && PostcardTextures[CurrentState - 1])
	{
		PostcardMaterial->SetTextureParameterValue(TextureParameterName, PostcardTextures[CurrentState - 1]);
	}
}

bool ALostMemory::IsBackFacingPlayer() const
{
	// While inspecting, the prop starts aligned with the camera, so its forward axis turning
	// against the camera forward means the player is looking at the back.
	if (!PlayerCharacter || !PlayerCharacter->GetFirstPersonCameraComponent())
	{
		return false;
	}
	const float Dot = FVector::DotProduct(GetActorForwardVector(), PlayerCharacter->GetFirstPersonCameraComponent()->GetForwardVector());
	return bShowingBack ? Dot < 0.5f : Dot < -0.5f;
}

void ALostMemory::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bLocked || !PlayerCharacter)
	{
		return;
	}

	USceneComponent* RootComp = GetRootComponent();
	if (!RootComp)
	{
		return;
	}

	const FRotator Target(0.f, 180.f, 0.f);
	const FRotator NewRotation = FMath::RInterpTo(RootComp->GetRelativeRotation(), Target, DeltaTime, AutoRotateSpeed);
	RootComp->SetRelativeRotation(NewRotation);

	if ((Target - NewRotation).GetNormalized().IsNearlyZero(3.0f))
	{
		SetActorTickEnabled(false);
		PlayerCharacter->Die();
	}
}

void ALostMemory::OnRotate(float delta)
{
	Super::OnRotate(delta);

	if (bLocked)
	{
		return;
	}

	const bool bBackNow = IsBackFacingPlayer();
	if (bBackNow && !bShowingBack)
	{
		CurrentState++;
		ApplyCurrentTexture();

		if (CurrentState >= PostcardTextures.Num())
		{
			bLocked = true;
			bIsInteractable = false;
		}
	}
	bShowingBack = bBackNow;
}