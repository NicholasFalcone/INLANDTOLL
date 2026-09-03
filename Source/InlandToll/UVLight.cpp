// Fill out your copyright notice in the Description page of Project Settings.

#include "UVLight.h"
#include "Components/SpotLightComponent.h"

AUVLight::AUVLight()
{
	PrimaryActorTick.bCanEverTick = true;

	UVSpotLight = CreateDefaultSubobject<USpotLightComponent>(TEXT("UVSpotLight"));
	UVSpotLight->SetupAttachment(RootComponent);
	UVSpotLight->SetRelativeLocationAndRotation(FVector(20.0f, 0.0f, 0.0f), FRotator::ZeroRotator);
	UVSpotLight->Intensity = 1000.0f;
	UVSpotLight->SetIntensityUnits(ELightUnits::Lumens);
	UVSpotLight->LightColor = FColor(140, 20, 255); // Hex #8C14FF, a rich UV purple
	UVSpotLight->AttenuationRadius = 800.0f;
	UVSpotLight->InnerConeAngle = 15.0f;
	UVSpotLight->OuterConeAngle = 35.0f;
	UVSpotLight->SetHiddenInGame(true); // Starts turned off
}

void AUVLight::BeginPlay()
{
	Super::BeginPlay();
}

void AUVLight::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AUVLight::OnUsed()
{
	Super::OnUsed();

	if (UVSpotLight)
	{
		bool bNewHiddenState = !UVSpotLight->bHiddenInGame;
		UVSpotLight->SetHiddenInGame(bNewHiddenState);
		
		UE_LOG(LogTemp, Warning, TEXT("UVLight: Light toggled to %s"), bNewHiddenState ? TEXT("OFF") : TEXT("ON"));
	}
}

void AUVLight::OnEquipped(ACharacter* InOwnerCharacter)
{
	Super::OnEquipped(InOwnerCharacter);

	// Ensure light starts turned off when equipped
	if (UVSpotLight)
	{
		UVSpotLight->SetHiddenInGame(true);
	}
}

void AUVLight::OnUnequipped()
{
	Super::OnUnequipped();

	// Turn off light when unequipped
	if (UVSpotLight)
	{
		UVSpotLight->SetHiddenInGame(true);
	}
}

