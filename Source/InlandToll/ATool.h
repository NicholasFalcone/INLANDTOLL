// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractable.h"
#include "ATool.generated.h"

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
