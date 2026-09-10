// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "BoxAnomaly.h"

ABoxAnomaly::ABoxAnomaly()
{
	// Create and configure the AddressWidget
	AddressWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("AddressWidget"));
	AddressWidget->SetupAttachment(RootComponent);
	AddressWidget->SetWidgetSpace(EWidgetSpace::World);
	AddressWidget->SetDrawSize(FVector2D(200.0f, 50.0f));
	AddressWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 100.0f)); // Adjust as needed
	// Create and configure the CodeWidget
	CodeWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("CodeWidget"));
	CodeWidget->SetupAttachment(RootComponent);
	CodeWidget->SetWidgetSpace(EWidgetSpace::World);
	CodeWidget->SetDrawSize(FVector2D(200.0f, 50.0f));
	CodeWidget->SetRelativeLocation(FVector(0.0f, 0.0f, 150.0f)); // Adjust as needed
}

void ABoxAnomaly::BeginPlay()
{
	Super::BeginPlay();
	LastSoundTime = 0;
}

void ABoxAnomaly::OnInteract()
{
	Super::OnInteract();
	// Update the widgets with the current address and code when interacting
	if (AddressWidget)
	{
		UpdateAddressWidget.Broadcast(Address);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("AddressWidget is not set in BoxAnomaly."));
	}
	if (CodeWidget)
	{
		UpdateCodeWidget.Broadcast(Code);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("CodeWidget is not set in BoxAnomaly."));
	}
}

void ABoxAnomaly::TriggerEventSound()
{
	if (UpdateSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, UpdateSound, GetActorLocation());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UpdateSound is not set in BoxAnomaly."));
	}
}

void ABoxAnomaly::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LastSoundTime += DeltaTime;

	if(LastSoundTime >= SoundCooldown)
	{
		TriggerEventSound();
		LastSoundTime = 0.0f;
	}
}