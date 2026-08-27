// Fill out your copyright notice in the Description page of Project Settings.

#include "Tablet.h"
#include "Variant_Horror/HorrorCharacter.h"
#include "Kismet/GameplayStatics.h"

ATablet::ATablet()
{
    // Constructor logic if needed
    TabletWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("TabletWidgetComponent"));
    TabletWidgetComponent->SetWidgetClass(TabletWidgetClass);
    TabletWidgetComponent->SetupAttachment(RootComponent);

    InteractionCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("InteractionCamera"));
    InteractionCamera->SetupAttachment(RootComponent);
}

void ATablet::BeginPlay()
{
    Super::BeginPlay();

    PlayerCharacter = Cast<AHorrorCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
}

void ATablet::OnInteract()
{
    Super::OnInteract();
    
    if(!PlayerCharacter) return;

    PlayerCharacter->EnterInspectionMode(nullptr, this); // Call the method to enter inspection mode
    APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController());
    if (PC)
    {
        PC->SetViewTargetWithBlend(this, 0.5f, EViewTargetBlendFunction::VTBlend_EaseInOut); // Blend to the tablet's camera
    }
}

void ATablet::OnEndInteract()
{
    Super::OnEndInteract();

    UE_LOG(LogTemp, Log, TEXT("Exiting inspection mode from tablet."));

    if(!PlayerCharacter) return;

    APlayerController* PC = Cast<APlayerController>(PlayerCharacter->GetController());
    if (PC)
    {
        PC->SetViewTargetWithBlend(PlayerCharacter, 0.5f, EViewTargetBlendFunction::VTBlend_EaseInOut); // Blend back to the player's camera
    }
}

void ATablet::UpdateAnomaly(const FInspectionData& currentInspectionData)
{
    // Assuming the Tablet has a method to update its display based on the inspection data
    // You would implement the logic here to update the tablet's UI or state
    UE_LOG(LogTemp, Log, TEXT("Updating Tablet with new anomaly data."));
    UTableUI* TableWidget = Cast<UTableUI>(TabletWidgetComponent->GetUserWidgetObject());
    if (TableWidget)
    {
        TableWidget->UpdateAnomaly(currentInspectionData);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("TabletWidgetComponent does not have a valid UTableUI instance."));
    }
}
