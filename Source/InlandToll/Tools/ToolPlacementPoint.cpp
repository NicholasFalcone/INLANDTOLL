// Fill out your copyright notice in the Description page of Project Settings.

#include "ToolPlacementPoint.h"
#include "Tool.h"
#include "Thermometer.h"
#include "UVLight.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"

AToolPlacementPoint::AToolPlacementPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	RootSceneComp = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	RootComponent = RootSceneComp;

	VisualizationMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VisualizationMesh"));
	VisualizationMesh->SetupAttachment(RootComponent);
	VisualizationMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	InteractionVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionVolume"));
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionVolume->SetCollisionObjectType(ECC_WorldStatic);
	InteractionVolume->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionVolume->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
}

void AToolPlacementPoint::BeginPlay()
{
	Super::BeginPlay();
}

void AToolPlacementPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool AToolPlacementPoint::TryPlaceTool(AATool* ToolToPlace)
{
	if (!bIsActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryPlaceTool FAIL: Placement point is not active"));
		return false;
	}
	
	if (!ToolToPlace)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryPlaceTool FAIL: ToolToPlace is nullptr"));
		return false;
	}
	
	if (bHasToolPlaced)
	{
		UE_LOG(LogTemp, Warning, TEXT("TryPlaceTool FAIL: Placement point already has a tool"));
		return false;
	}

	// Get tool type - we need to determine this from the tool
	EToolType ToolType = EToolType::ETT_None;
	
	if (ToolToPlace->IsA<AThermometer>())
	{
		ToolType = EToolType::ETT_Thermometer;
	}
	else if (ToolToPlace->IsA<AUVLight>())
	{
		ToolType = EToolType::ETT_UVLight;
	}

	// Check if this placement point accepts this tool type
	if (!CanAcceptToolType(ToolType))
	{
		UE_LOG(LogTemp, Warning, TEXT("TryPlaceTool FAIL: Placement point does not accept tool type %d"), (int32)ToolType);
		return false;
	}

	// Drop and place the tool
	ToolToPlace->OnDropped(nullptr);
	ToolToPlace->SetActorLocation(GetToolPlacementLocation());
	ToolToPlace->SetActorRotation(GetToolPlacementRotation());

	if (ToolToPlace->FindComponentByClass<UStaticMeshComponent>())
	{
		UStaticMeshComponent* MeshComp = ToolToPlace->FindComponentByClass<UStaticMeshComponent>();
		MeshComp->SetSimulatePhysics(false);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	PlacedTool = ToolToPlace;
	bHasToolPlaced = true;

	// Subscribe to the tool's OnToolEquipped event to know when it's picked up again
	if (ToolToPlace->OnToolEquipped.IsBound())
	{
		ToolToPlace->OnToolEquipped.AddDynamic(this, &AToolPlacementPoint::OnPlacedToolEquipped);
	}

	UE_LOG(LogTemp, Warning, TEXT("TryPlaceTool PLACED: %s"), *ToolToPlace->GetName());

	if (OnToolPlaced.IsBound())
	{
		OnToolPlaced.Broadcast(ToolToPlace, this);
	}

	return true;
}

AATool* AToolPlacementPoint::RemoveTool()
{
	if (!bHasToolPlaced || !PlacedTool)
	{
		return nullptr;
	}

	AATool* RemovedTool = PlacedTool;
	PlacedTool = nullptr;
	bHasToolPlaced = false;

	if (RemovedTool->FindComponentByClass<UStaticMeshComponent>())
	{
		UStaticMeshComponent* MeshComp = RemovedTool->FindComponentByClass<UStaticMeshComponent>();
		MeshComp->SetSimulatePhysics(true);
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}

	UE_LOG(LogTemp, Warning, TEXT("TryPlaceTool REMOVED: %s"), *RemovedTool->GetName());

	if (OnToolRemoved.IsBound())
	{
		OnToolRemoved.Broadcast(RemovedTool, this);
	}

	return RemovedTool;
}

bool AToolPlacementPoint::CanAcceptToolType(EToolType ToolType) const
{
	if (!bIsActive)
	{
		return false;
	}

	if (AcceptedToolType == EToolType::ETT_Any)
	{
		return true;
	}

	return AcceptedToolType == ToolType;
}

void AToolPlacementPoint::Highlight()
{
	if (VisualizationMesh && HighlightMaterial)
	{
		VisualizationMesh->SetMaterial(0, HighlightMaterial);
	}

	if (bHighlightPlacedTool && PlacedTool)
	{
		PlacedTool->OnHighlight();
	}

	if (OnPlacementPointHighlighted.IsBound())
	{
		OnPlacementPointHighlighted.Broadcast();
	}
}

void AToolPlacementPoint::Unhighlight()
{
	if (VisualizationMesh)
	{
		VisualizationMesh->SetMaterial(0, nullptr);
	}

	if (bHighlightPlacedTool && PlacedTool)
	{
		PlacedTool->OnUnhighlight();
	}

	if (OnPlacementPointUnhighlighted.IsBound())
	{
		OnPlacementPointUnhighlighted.Broadcast();
	}
}

FVector AToolPlacementPoint::GetToolPlacementLocation() const
{
	return GetActorLocation() + GetActorRotation().RotateVector(ToolPlacementOffset);
}

FRotator AToolPlacementPoint::GetToolPlacementRotation() const
{
	return GetActorRotation() + ToolPlacementRotation;
}

void AToolPlacementPoint::OnPlacedToolEquipped()
{
	if (PlacedTool && PlacedTool->OnToolEquipped.IsBound())
	{
		PlacedTool->OnToolEquipped.RemoveDynamic(this, &AToolPlacementPoint::OnPlacedToolEquipped);
	}

	PlacedTool = nullptr;
	bHasToolPlaced = false;

	UE_LOG(LogTemp, Warning, TEXT("TryPlaceTool REMOVED: Tool re-equipped by player"));
}
