// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/AudioComponent.h"

#include "BaseInteractable.h"
#include "Tool.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnToolUsed);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnToolEquipped);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnToolDropped);

UCLASS()
class INLANDTOLL_API AATool : public ABaseInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AATool();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool")
	FVector SocketOffset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool")
	FRotator SocketRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Tool")
	FName AttachSocketName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* OnUsedAudioComp;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UAudioComponent* OnEquippedAudioComp;

public:
	UPROPERTY(BlueprintAssignable, Category = "Tool")
	FOnToolUsed OnToolUsed;

	UPROPERTY(BlueprintAssignable, Category = "Tool")
	FOnToolEquipped OnToolEquipped;

	UPROPERTY(BlueprintAssignable, Category = "Tool")
	FOnToolDropped OnToolDropped;
public:	

	UPROPERTY(BlueprintReadOnly, Category = "Tool")
	bool bIsEquipped;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void OnEquipped(class ACharacter* InOwnerCharacter);
	
	virtual void OnUnequipped();

	virtual void OnUsed();

	virtual void OnDropped(class ACharacter* InDroppedByCharacter);

	virtual void OnInteract() override;
};
