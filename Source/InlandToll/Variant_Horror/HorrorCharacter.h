// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InlandTollCharacter.h"
#include "BPC_Inventory.h"
#include "InteractionComponent.h"
#include "HorrorCharacter.generated.h"

class USpotLightComponent;
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

	/** Player light source */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USpotLightComponent* SpotLight;

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

	UBPC_Inventory* InventoryComponent;

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
	void DoStartToggleLight();

	/** Stops sprinting behavior */
	UFUNCTION(BlueprintCallable, Category = "Input")
	void DoEndToggleLight();

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
};
