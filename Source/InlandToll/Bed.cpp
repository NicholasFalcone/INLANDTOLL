// Fill out your copyright notice in the Description page of Project Settings.


#include "Bed.h"
#include "Desk.h"
#include "Kismet/GameplayStatics.h"


ABed::ABed()
{
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("SleepingCamera"));
	CameraComponent->SetupAttachment(RootComponent);
}

void ABed::BeginPlay()
{
	Super::BeginPlay();
    if(ADesk* DeskInstance =  Cast<ADesk>(UGameplayStatics::GetActorOfClass(GetWorld(), ADesk::StaticClass())))
    {
        DeskInstance->OnDailyInspectionLimitReached.AddDynamic(this, &ABed::HandleDailyInspectionLimitReached);
    }
}

void ABed::HandleDailyInspectionLimitReached()
{
    // Implement logic to handle daily inspection limit reached
    bIsInteractable = true;
}

void ABed::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ABed::OnInteract()
{
	// Implement interaction logic here
    if (CameraComponent)
    {
        GetWorld()->GetFirstPlayerController()->SetViewTargetWithBlend(this);
        bIsInteractable = false;
        OnPlayerSleeping.Broadcast();
    }
}
