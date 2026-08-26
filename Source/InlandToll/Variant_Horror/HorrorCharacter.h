// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "InlandTollCharacter.h"
#include "BPC_Inventory.h"
#include "Tablet.h"
#include "InteractionComponent.h"
#include "HorrorCharacter.generated.h"

class USpotLightComponent;
class UInteractionComponent;
class UInputAction;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FUpdateSprintMeterDelegate, float, Percentage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSprintStateChangedDelegate, bool, bSprinting);

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
	UPROPERTY(EditAnywhere, Category ="Input")
	UInputAction* SprintAction;

	/** Fire weapon input action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* UseItem;

	/** Interact input action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* InteractAction;

	/** Interact input action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* CrouchAction;

	
	/** Interact input action */
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* OpenTabletAction;

	/** If true, we're sprinting */
	bool bSprinting = false;

	/** If true, we're recovering stamina */
	bool bRecovering = false;

	/** Default walk speed when not sprinting or recovering */
	UPROPERTY(EditAnywhere, Category="Walk")
	float WalkSpeed = 250.0f;

	/** Time interval for sprinting stamina ticks */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, ClampMax = 1, Units = "s"))
	float SprintFixedTickTime = 0.03333f;

	/** Sprint stamina amount. Maxes at SprintTime */
	float SprintMeter = 0.0f;

	/** How long we can sprint for, in seconds */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float SprintTime = 3.0f;

	/** Walk speed while sprinting */
	UPROPERTY(EditAnywhere, Category="Sprint", meta = (ClampMin = 0, ClampMax = 10, Units = "cm/s"))
	float SprintSpeed = 600.0f;

	/** Walk speed while recovering stamina */
	UPROPERTY(EditAnywhere, Category="Recovery", meta = (ClampMin = 0, ClampMax = 10, Units = "cm/s"))
	float RecoveringWalkSpeed = 150.0f;

	/** Time it takes for the sprint meter to recover */
	UPROPERTY(EditAnywhere, Category="Recovery", meta = (ClampMin = 0, ClampMax = 10, Units = "s"))
	float RecoveryTime = 0.0f;

	/** Sprint tick timer */
	FTimerHandle SprintTimer;
	UBPC_Inventory* InventoryComponent;

public:

	/** Delegate called when the sprint meter should be updated */
	FUpdateSprintMeterDelegate OnSprintMeterUpdated;

	/** Delegate called when we start and stop sprinting */
	FSprintStateChangedDelegate OnSprintStateChanged;

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
	void DoStartSprint();

	/** Stops sprinting behavior */
	UFUNCTION(BlueprintCallable, Category="Input")
	void DoEndSprint();

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

	void DoStartUseTable();
	void DoEndUseTable();

	/** Called while sprinting at a fixed time interval */
	void SprintFixedTick();



public:
	// --- Inspection Mode ---
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection")
	bool bIsInspecting = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection")
	class AInspectionProp* CurrentInspectedProp;

	UFUNCTION(BlueprintCallable, Category = "Inspection")
	void EnterInspectionMode(class AInspectionProp* PropToInspect);

	UFUNCTION(BlueprintCallable, Category = "Inspection")
	void ExitInspectionMode();

	virtual void DoAim(float Yaw, float Pitch) override;
	virtual void DoMove(float Right, float Forward) override;

	// Distanza dall'occhio della camera per l'oggetto in ispezione
	UPROPERTY(EditAnywhere, Category = "Inspection")
	float InspectionOffset = 50.0f;

	UPROPERTY(EditAnywhere, Category = "Inspection")
	TSubclassOf<ATablet> TabletClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inspection")
	ATablet* EquippedTablet = nullptr;
	UFUNCTION(BlueprintImplementableEvent, Category = "Custom Events")
	void CallOnTabletEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category = "Custom Events")
	void CallOnTabletUnequipped();
};
