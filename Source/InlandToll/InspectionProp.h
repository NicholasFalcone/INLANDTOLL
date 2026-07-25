// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseInteractable.h"
#include "InspectionProp.generated.h"

/**
 * 
 */
UCLASS()
class INLANDTOLL_API AInspectionProp : public ABaseInteractable
{
	GENERATED_BODY()
	
public:
	virtual void OnInteract() override;

	// Salva la posizione e rotazione originale prima dell'ispezione
	FVector OriginalLocation;
	FRotator OriginalRotation;

	// Salva il riferimento al componente e alla socket a cui l'oggetto era attaccato
	UPROPERTY()
	USceneComponent* OriginalParentComponent;
	FName OriginalSocketName;

	void SetIsInspecting(bool bNewInspecting);
};
