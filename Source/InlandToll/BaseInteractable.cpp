// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseInteractable.h"
#include "Components/WidgetComponent.h"
#include "InteractionPrompt.h" // Replace with the actual header for UInteractionPrompt if different

// Sets default values
ABaseInteractable::ABaseInteractable()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// 3. Creo e configuro il WidgetComponent
	// Note: CreateDefaultSubobject must be called in the Constructor, not in BeginPlay.
	// You will need to declare 'class UWidgetComponent* InteractionWidget;' in your header file.
	InteractionWidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionWidgetComp"));
	InteractionWidgetComp->SetupAttachment(RootComponent);

	// Posizione sopra l'oggetto
	InteractionWidgetComp->SetRelativeLocation(InteractionPromptData.InteractionPromptOffset);

	// Spazio dello schermo o del mondo
	// EWidgetSpace::Screen = La UI mantiene dimensioni fisse ed � sempre rivolta verso la camera
	// EWidgetSpace::World  = La UI � un vero oggetto 3D posizionato nel mondo
	InteractionWidgetComp->SetWidgetSpace(EWidgetSpace::World);
	InteractionWidgetComp->SetDrawSize(FVector2D(200.0f, 50.0f));

	// Di base nascondiamo la UI fino a quando il giocatore non si avvicina
	InteractionWidgetComp->SetVisibility(false);
}

// Called when the game starts or when spawned
void ABaseInteractable::BeginPlay()
{
	Super::BeginPlay();


	if (InteractionWidgetComp && InteractionPromptClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("InteractionWidgetComp and InteractionPromptClass are valid in BeginPlay"));
		InteractionWidgetComp->SetWidgetClass(InteractionPromptClass);

		// Accediamo all'istanza del widget creata dal componente invece che al CDO
		UInteractionPrompt* PromptWidget = Cast<UInteractionPrompt>(InteractionWidgetComp->GetUserWidgetObject());
		if (PromptWidget)
		{
			UE_LOG(LogTemp, Warning, TEXT("PromptWidget is valid in BeginPlay"));
			PromptWidget->SetPromptText(InteractionPromptData.InteractionPrompt);
		}
		else{
			UE_LOG(LogTemp, Warning, TEXT("PromptWidget is NULL in BeginPlay"));
		}
		
		InteractionWidgetComp->SetRelativeLocation(InteractionPromptData.InteractionPromptOffset);
		InteractionWidgetComp->SetVisibility(false);
	}
}

// Called every frame
void ABaseInteractable::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABaseInteractable::OnHighlight()
{
	InteractionWidgetComp->SetVisibility(true);
}

void ABaseInteractable::OnUnhighlight()
{
	InteractionWidgetComp->SetVisibility(false);
}

void ABaseInteractable::OnInteract()
{
	if (OnInteractDelegate.IsBound())
	{
		OnInteractDelegate.Broadcast();
	}
	CallOnInteraction();
}
