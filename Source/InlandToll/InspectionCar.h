// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "InspectionCar.generated.h"

UCLASS()
class INLANDTOLL_API AInspectionCar : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInspectionCar();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* CarMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float movementSpeed = 100.0f; // Adjust this value to control the speed of the car

	FVector startLocation;
	FVector endLocation;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void MoveCar(float DeltaTime);

	void InitializeCarData(USkeletalMesh* Mesh, FString SocketName, UStaticMesh* InspectionMesh)
	{
		if (CarMesh && Mesh)
		{
			CarMesh->SetSkeletalMesh(Mesh);
			// You can also set the socket name and inspection mesh if needed
		}
	}

	void InitializeCarMovement(const FVector& Start, const FVector& End)
	{
		startLocation = Start;
		endLocation = End;
		SetActorLocation(startLocation);
	}

};
