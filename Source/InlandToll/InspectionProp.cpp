// Fill out your copyright notice in the Description page of Project Settings.


#include "InspectionProp.h"
#include "Variant_Horror/HorrorCharacter.h"
#include "Kismet/GameplayStatics.h"

void AInspectionProp::OnInteract()
{
    Super::OnInteract();

    AHorrorCharacter* Player = Cast<AHorrorCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
    if (Player)
    {
        Player->EnterInspectionMode(this);
    }
}

void AInspectionProp::SetIsInspecting(bool bNewInspecting)
{
    if (MeshComp)
    {
        if (bNewInspecting)
        {
            // Disabilita collisioni durante l'ispezione per evitare problemi fisici
            MeshComp->SetSimulatePhysics(false);
            MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
        }
        else
        {
            MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
        }
    }
}


void AInspectionProp::OnEndInteract()
{
    Super::OnEndInteract();
    
    SetIsInspecting(false); // Ensure the prop is no longer in inspecting state when interaction ends
}
