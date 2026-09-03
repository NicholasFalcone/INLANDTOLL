// Fill out your copyright notice in the Description page of Project Settings.

#include "Thermometer.h"
#include "Components/TextRenderComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "CollisionQueryParams.h"
#include "Engine/World.h"

AThermometer::AThermometer()
{
	PrimaryActorTick.bCanEverTick = true;

	TemperatureTextComp = CreateDefaultSubobject<UTextRenderComponent>(TEXT("TemperatureTextComp"));
	TemperatureTextComp->SetupAttachment(RootComponent);
	TemperatureTextComp->SetRelativeLocation(FVector(10.0f, 0.0f, 15.0f));
	TemperatureTextComp->SetRelativeRotation(FRotator(0.0f, 180.0f, 0.0f)); // Face the player mesh/view
	TemperatureTextComp->SetText(FText::FromString(TEXT("20.0 C")));
	TemperatureTextComp->SetTextRenderColor(FColor::Green);
	TemperatureTextComp->SetHorizontalAlignment(EHTA_Center);
	TemperatureTextComp->SetVerticalAlignment(static_cast<EVerticalTextAligment>(2));
	TemperatureTextComp->SetWorldSize(6.0f);

	AmbientTemperature = 20.0f;
	TargetTemperature = 20.0f;
	CurrentTemperature = 20.0f;
	InterpSpeed = 3.0f;
	TraceDistance = 800.0f;
}

void AThermometer::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentTemperature = AmbientTemperature;
	TargetTemperature = AmbientTemperature;
}

void AThermometer::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsEquipped)
	{
		// Smoothly interpolate current temperature to target for realistic digital sensor lag
		CurrentTemperature = FMath::FInterpTo(CurrentTemperature, TargetTemperature, DeltaTime, InterpSpeed);

		// Update the display text
		if (TemperatureTextComp)
		{
			TemperatureTextComp->SetText(FText::FromString(FString::Printf(TEXT("%.1f C"), CurrentTemperature)));
			
			// Color code: Green for ambient/warm, Cyan/Blue for cold, Red for hot
			if (CurrentTemperature < 8.0f)
			{
				TemperatureTextComp->SetTextRenderColor(FColor::Cyan);
			}
			else if (CurrentTemperature > 38.0f)
			{
				TemperatureTextComp->SetTextRenderColor(FColor::Red);
			}
			else
			{
				TemperatureTextComp->SetTextRenderColor(FColor::Green);
			}
		}

		// Update target temperature from raycast
		UpdateTargetTemperature();
	}
}

void AThermometer::UpdateTargetTemperature()
{
	if (!GetWorld()) return;

	float NewTarget = AmbientTemperature;

	ACharacter* OwnerChar = Cast<ACharacter>(GetOwner());
	if (OwnerChar && OwnerChar->GetController())
	{
		FVector ViewLocation;
		FRotator ViewRotation;
		OwnerChar->GetController()->GetPlayerViewPoint(ViewLocation, ViewRotation);

		FVector TraceEnd = ViewLocation + (ViewRotation.Vector() * TraceDistance);
		FHitResult HitResult;
		FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(ThermometerTrace), true, OwnerChar);
		TraceParams.AddIgnoredActor(this);

		if (GetWorld()->LineTraceSingleByChannel(HitResult, ViewLocation, TraceEnd, ECC_Visibility, TraceParams))
		{
			AActor* HitActor = HitResult.GetActor();
			if (HitActor)
			{
				// Detect anomaly, cold or hot spots by tags typical of horror games
				if (HitActor->ActorHasTag("Cold") || HitActor->ActorHasTag("Anomaly"))
				{
					NewTarget = 2.4f; // Cold room or entity
				}
				else if (HitActor->ActorHasTag("Freezing"))
				{
					NewTarget = -4.5f; // Freezing anomaly evidence
				}
				else if (HitActor->ActorHasTag("Hot"))
				{
					NewTarget = 45.2f; // Warm anomaly or object
				}
			}
		}
	}

	TargetTemperature = NewTarget;
}

void AThermometer::OnUsed()
{
	Super::OnUsed();
	
	// Active use: make a quick sensor flash or trace update
	UE_LOG(LogTemp, Warning, TEXT("Thermometer used (Beep!). Current: %.1f C, Target: %.1f C"), CurrentTemperature, TargetTemperature);
	
	// Force immediate update to display active scanning feel
	UpdateTargetTemperature();
}

void AThermometer::OnEquipped(ACharacter* InOwnerCharacter)
{
	Super::OnEquipped(InOwnerCharacter);

	// Start reading from ambient
	CurrentTemperature = AmbientTemperature;
	TargetTemperature = AmbientTemperature;
}

void AThermometer::OnUnequipped()
{
	Super::OnUnequipped();
}
