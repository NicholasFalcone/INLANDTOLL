// Fill out your copyright notice in the Description page of Project Settings.

#include "ATool.h"

// Sets default values
AATool::AATool()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AATool::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AATool::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AATool::OnEquipped()
{
	// Implement logic for when the tool is equipped
}

void AATool::OnUnequipped()
{
	// Implement logic for when the tool is unequipped
}

void AATool::OnUsed()
{
	// Implement logic for when the tool is used
}

