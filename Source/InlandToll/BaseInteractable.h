// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "InteractionPrompt.h"
#include "InteractionPromptData.h"
#include "ST_DialogueLine.h"
#include "BaseInteractable.generated.h"

class UWidgetComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInteractDelegate);

UCLASS()
class INLANDTOLL_API ABaseInteractable : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseInteractable();

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
	FOnInteractDelegate OnInteractDelegate;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category = "Prompt Data")
	FInteractionPromptData InteractionPromptData;
	
	UPROPERTY(EditAnywhere, Category = "Prompt Data")
	TSubclassOf<UInteractionPrompt> InteractionPromptClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Prompt Data")
	UWidgetComponent* InteractionWidgetComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
	TArray<FST_DialogueLine> InteractionDialogueLines;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void OnHighlight();
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void OnUnhighlight();
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void OnInteract();
	UFUNCTION(BlueprintImplementableEvent, Category = "Custom Events")
	void CallOnInteraction();
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void OnEndInteract();
	UFUNCTION(BlueprintImplementableEvent, Category = "Custom Events")
	void CallOnEndInteraction();
};
