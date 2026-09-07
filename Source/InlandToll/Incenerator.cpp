// Fill out your copyright notice in the Description page of Project Settings.


#include "Incenerator.h"
#include "Crane.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AIncenerator::AIncenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SplinePath = CreateDefaultSubobject<USplineComponent>(TEXT("Crane_SplinePath"));
	RootComponent = SplinePath;
}

// Called when the game starts or when spawned
void AIncenerator::BeginPlay()
{
	Super::BeginPlay();
	
	if (ACrane* Crane = Cast<ACrane>(UGameplayStatics::GetActorOfClass(GetWorld(), ACrane::StaticClass())))
	{
		CraneInstance = Crane;
		CraneInstance->InitializeCrane(SplinePath);
		CraneInstance->SetOwnerIncenerator(this);
	}
}

// Called every frame
void AIncenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AIncenerator::StartInceneratorSequence()
{
	if (CraneInstance)
	{
		CraneInstance->StartSequence();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No CraneInstance assigned in AIncenerator!"));
	}
}

void AIncenerator::NotifyAnomalyDropped(AActor* Anomaly)
{
	if (Anomaly)
	{
		// Call Blueprint implementable event first so VFX/SFX are triggered
		OnAnomalyDropped(Anomaly);

		// Burn (destroy) the anomaly after a 2.0-second delay so it's visible falling in
		FTimerHandle BurnTimer;
		FTimerDelegate BurnDelegate;
		BurnDelegate.BindUObject(this, &AIncenerator::BurnAnomaly, Anomaly);
		GetWorld()->GetTimerManager().SetTimer(BurnTimer, BurnDelegate, 2.0f, false);
	}
}

void AIncenerator::BurnAnomaly(AActor* Anomaly)
{
	if (IsValid(Anomaly))
	{
		UE_LOG(LogTemp, Log, TEXT("Incinerator is burning anomaly: %s"), *Anomaly->GetName());
		Anomaly->Destroy();
	}
}

