// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "HorrorCharacter.h"

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
    if (!World) return nullptr;

    // 1. Definisci punto di partenza, arrivo e raggio della sfera (in cm)
    FVector Start = OwnerCharacter ? OwnerCharacter->GetActorLocation() : FVector::ZeroVector;
    FVector End = Start + (OwnerCharacter ? OwnerCharacter->GetActorForwardVector() * InteractionDistance : FVector::ForwardVector * InteractionDistance); // 5 metri in avanti
    float Radius = 50.0f;

    // 2. Prepara il risultato dell'impatto e la forma sferica
    FHitResult HitResult;
    FCollisionShape SphereShape = FCollisionShape::MakeSphere(Radius);

    // 3. Imposta i parametri di query (es. ignorare se stessi)
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerCharacter); // Ignora questo Actor
    QueryParams.bTraceComplex = false; // true se vuoi verificare contro la geometria complessa/mesh

    // 4. Esegui lo Sweep (Sphere Cast)
    bool bHit = World->SweepSingleByChannel(
        HitResult,
        Start,
        End,
        FQuat::Identity, // Nessuna rotazione (la sfera � simmetrica)
        ECC_Visibility,  // Canale di collisione (es. ECC_Camera, ECC_Pawn, ECC_WorldDynamic)
        SphereShape,
        QueryParams
    );

    // 5. Debug visivo nell'editor / gioco
    FColor DebugColor = bHit ? FColor::Red : FColor::Green;

    if (bHit)
    {
        AActor* HitActor = HitResult.GetActor();
		ABaseInteractable* Interactable = Cast<ABaseInteractable>(HitActor);
        if (Interactable)
        {
            UE_LOG(LogTemp, Log, TEXT("Sphere Cast ha colpito: %s"), *HitActor->GetName());
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


