// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "HorrorCharacter.h"
#include "Camera/CameraComponent.h"
#include "DrawDebugHelpers.h"

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void UInteractionComponent::Init(AHorrorCharacter* InOwnerCharacter)
{
    OwnerCharacter = InOwnerCharacter;
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

ABaseInteractable* UInteractionComponent::CheckForInteractables()
{
    UWorld* World = GetWorld();
    if (!World || !OwnerCharacter) return nullptr;

    UCameraComponent* Camera = OwnerCharacter->GetFirstPersonCameraComponent();
    if (!Camera) return nullptr;

    // 1. Definisci punto di partenza e arrivo basati sulla Camera
    FVector Start = Camera->GetComponentLocation();
    FVector End = Start + (Camera->GetForwardVector() * InteractionDistance);

    // 2. Prepara il risultato dell'impatto
    FHitResult HitResult;

    // 3. Imposta i parametri di query (es. ignorare se stessi)
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerCharacter); // Ignora questo Actor
    QueryParams.bTraceComplex = false; // per performance, usiamo collisioni semplici

    // 4. Esegui il Raycast (Line Trace)
    bool bHit = World->LineTraceSingleByChannel(
        HitResult,
        Start,
        End,
        ECC_Visibility,  // Canale di collisione (es. ECC_Visibility)
        QueryParams
    );

    // 5. Debug visivo opzionale nell'editor per verificare la direzione di interazione
    // DrawDebugLine(World, Start, End, bHit ? FColor::Green : FColor::Red, false, 0.1f, 0, 2.0f);

    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
        ABaseInteractable* Interactable = Cast<ABaseInteractable>(HitActor);
        if (Interactable)
        {
            return Interactable;
        }
    }

	return nullptr;
}






// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ABaseInteractable* Interactable = CheckForInteractables();

    if (Interactable) {
        if(CurrentInteractable != Interactable) {
            
            if(CurrentInteractable) {
                CurrentInteractable->OnUnhighlight();
            }
            CurrentInteractable = Interactable;
            CurrentInteractable->OnHighlight();
		}
    }
    else 
    {
        if (CurrentInteractable) {
            CurrentInteractable->OnUnhighlight();
            CurrentInteractable = nullptr;
        }
    }
}


