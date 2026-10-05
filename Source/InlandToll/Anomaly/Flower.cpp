// Fill out your copyright notice in the Description page of Project Settings.


#include "Anomaly/Flower.h"

AFlower::AFlower()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AFlower::BeginPlay()
{
	Super::BeginPlay();
}

void AFlower::OnRotate(float delta)
{
}

void AFlower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
        UChildActorComponent* Petal = Cast<UChildActorComponent>(Petals.Pop());
        Petal->SetSimulatePhysics(true);
        Petal->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		FTimerHandle PetalTimer;
		FTimerDelegate PetalDropDelegate;
		PetalDropDelegate.BindUObject(this, &AFlower::DestroyPetal, Petal);
		GetWorld()->GetTimerManager().SetTimer(PetalTimer, PetalDropDelegate, 2.0f, false);
	
    }
}

void AFlower::DestroyPetal(UChildActorComponent* Petal)
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