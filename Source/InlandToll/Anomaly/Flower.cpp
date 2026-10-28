// Fill out your copyright notice in the Description page of Project Settings.


#include "Anomaly/Flower.h"
#include "TimerManager.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

AFlower::AFlower()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlower::BeginPlay()
{
	Super::BeginPlay();

    TArray<UStaticMeshComponent*> AllPetals;
    GetComponents<UStaticMeshComponent>(AllPetals); 
    // 2. Filtra per Tag
    for (UStaticMeshComponent* Petal : AllPetals)
    {
        if (Petal && Petal->ComponentHasTag(FName("Petal")))
        {
            Petals.Add(Petal);
        }
        else if (Petal && !Head && Petal->ComponentHasTag(FName("Head")))
        {
            Head = Petal;
        }
    }

    if (!Head)
    {
        UE_LOG(LogTemp, Warning, TEXT("Flower head not found: tag a static mesh component with 'Head'."));
    }
}

void AFlower::UpdateHeadLookAt()
{
    if (!Head || !PlayerCharacter)
    {
        return;
    }

    const FVector Target = PlayerCharacter->GetFirstPersonCameraComponent()
        ? PlayerCharacter->GetFirstPersonCameraComponent()->GetComponentLocation()
        : PlayerCharacter->GetActorLocation();

    const FRotator LookAt = UKismetMathLibrary::FindLookAtRotation(Head->GetComponentLocation(), Target);
    Head->SetWorldRotation(LookAt + HeadLookAtOffset);
}

void AFlower::OnRotate(float delta)
{
}

void AFlower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    UpdateHeadLookAt();
    if(HasPetals())
    {
        if(CurrentTime >= PetalLossRateTime)
        {
            LosePetal();
            CurrentTime = 0;
        }
        else
        {
            CurrentTime += DeltaTime;
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No more petals left."));
        CurrentTime = 0;
    }
}

void AFlower::LosePetal()
{
	// Implement petal loss logic here
    if(HasPetals())
    {
        UStaticMeshComponent* Petal = Petals.Pop();
        // Petal->SetSimulatePhysics(true);
        Petal->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		FTimerHandle PetalTimer;
		FTimerDelegate PetalDropDelegate;
        Petal->SetSimulatePhysics(true);
		PetalDropDelegate.BindUObject(this, &AFlower::DestroyPetal, Petal);
		GetWorld()->GetTimerManager().SetTimer(PetalTimer, PetalDropDelegate, 2.0f, false);
	
    }
}

void AFlower::DestroyPetal(UStaticMeshComponent* Petal)
{   
	if(Petal)
	{
		Petal->DestroyComponent();
	}
}

bool AFlower::HasPetals()
{
	return Petals.Num() > 0;
}