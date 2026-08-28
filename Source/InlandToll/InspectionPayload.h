// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SplineComponent.h"
#include "InspectionProp.h"
#include "Engine/StaticMesh.h"
#include "InspectionPayload.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCarReachedEnd, AInspectionPayload*, ReachedCar);

UCLASS(Blueprintable)
class INLANDTOLL_API AInspectionPayload : public APawn
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AInspectionPayload();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	UStaticMeshComponent* CarMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float movementSpeed = 500.0f; // Adjust this value to control the speed of the car

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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
	USceneComponent* InteractablePosition;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "State")
	bool bIsMoving = false;

	bool bHasReachedEnd = false;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void MoveCar(float DeltaTime);

	UFUNCTION(BlueprintCallable, Category = "Movement")
	void ResumeMovementToEnd();

	UFUNCTION(BlueprintCallable, Category = "Car")
	void InitializeCarData(UStaticMesh* Mesh, FString SocketName, TSubclassOf<AInspectionProp> PropClass);

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

	void SetInspectionProp(TSubclassOf<AInspectionProp> PropClass, FString SocketName);
};
