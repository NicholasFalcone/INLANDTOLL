// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InlandTollCharacter.h"
#include "BPC_Inventory.h"
#include "InteractionComponent.h"
#include "HorrorCharacter.generated.h"


class UInteractionComponent;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDiedDelegate);
/**
 *  Simple first person horror character
 *  Provides stamina-based sprinting
 */
UCLASS(abstract)
class INLANDTOLL_API AHorrorCharacter : public AInlandTollCharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UInteractionComponent* InteractionComponent;
	
protected:
	/** Fire weapon input action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* UseItem;

	/** Interact input action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

	/** Interact input action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CrouchAction;

	/** Default walk speed when not sprinting or recovering */
	UPROPERTY(EditAnywhere, Category="Walk")
	float WalkSpeed = 250.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UBPC_Inventory* InventoryComponent;

	/** Drop input action (usually bound to Q) */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* DropAction;

public:
	UFUNCTION(BlueprintCallable, Category = "Tools")
	void EquipToolFromGround(class AATool* NewTool);

	UFUNCTION(BlueprintCallable, Category = "Tools")
	void DoDropTool();

protected:

	/** Constructor */
	AHorrorCharacter();

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Gameplay cleanup */
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

	/** Set up input action bindings */
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

protected:

	/** Starts sprinting behavior */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoStartUsingTool();

	/** Stops sprinting behavior */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoEndUsingTool();

	void DoStartInteract();
	void DoEndInteract();

	void DoStartCrouch();
	void DoEndCrouch();

public:
	// --- Inspection Mode ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection")
	bool bIsInspecting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection")
	class AInspectionProp* CurrentInspectedProp;

	class ABaseInteractable* CurrentInteractable;

	UFUNCTION(BlueprintCallable, Category = "Inspection")
	void EnterInspectionMode(class AInspectionProp* PropToInspect, class ABaseInteractable* Interactable = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Inspection")
	void ExitInspectionMode();

	virtual void DoAim(float Yaw, float Pitch) override;
	virtual void DoMove(float Right, float Forward) override;

	// Distanza dall'occhio della camera per l'oggetto in ispezione
	UPROPERTY(EditAnywhere, Category = "Inspection")
	float InspectionOffset = 50.0f;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnPlayerDiedDelegate OnPlayerDied;

	UFUNCTION()
	void DisableMovement();

	UFUNCTION()
	void EnableMovement();

	UFUNCTION()
	void HidePlayerMesh();

	UFUNCTION()
	void ShowPlayerMesh();

	void Die();
};
