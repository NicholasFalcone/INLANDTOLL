// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SplineComponent.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/StaticMesh.h"
#include "InspectionCar.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCarReachedEnd, AInspectionCar*, ReachedCar);

UCLASS()
class INLANDTOLL_API AInspectionCar : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInspectionCar();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* CarMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UChildActorComponent* DoorInteraction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UChildActorComponent* CargoInteraction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interaction")
	UChildActorComponent* InspectionInteraction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float movementSpeed = 100.0f; // Adjust this value to control the speed of the car

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	class USplineComponent* TargetSpline;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	int32 StopAtSplineIndex = -1;

	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnCarReachedEnd OnCarReachedEnd;

	float DistanceAlongSpline = 0.0f;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void HandleDoorInteracted();

	UFUNCTION()
	void HandleCargoInteracted();

	UFUNCTION()
	void HandleInspectionInteracted();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bDoorOpen = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bCargoOpen = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsMoving = false;

	bool bHasReachedEnd = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void MoveCar(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ResumeMovementToEnd();

	void InitializeCarData(USkeletalMesh* Mesh, FString SocketName, UStaticMesh* InspectionMesh)
	{
		if (CarMesh && Mesh)
		{
			CarMesh->SetSkeletalMesh(Mesh);
			SetInspectionMesh(InspectionMesh, SocketName);
		}
	}

	void InitializeCarMovement(USplineComponent* InSpline)
	{
		TargetSpline = InSpline;
		DistanceAlongSpline = 0.0f;
		if (TargetSpline)
		{
			SetActorLocationAndRotation(TargetSpline->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World), 
										TargetSpline->GetRotationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World));
		}
		StopAtSplineIndex = 2; // Set the index of the spline point where the car should stop
	}

	void SetInspectionMesh(UStaticMesh* InspectionMesh, FString SocketName);
};
