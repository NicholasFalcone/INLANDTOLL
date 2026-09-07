// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Crane.generated.h"

class USkeletalMeshComponent;
class AIncenerator;

UENUM(BlueprintType)
enum class ECraneState : uint8
{
	Idle,
	MovingForward,
	Descending,
	Hooking,
	Ascending,
	MovingBackward,
	Dropping
};

UCLASS()
class INLANDTOLL_API ACrane : public AActor
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Rig;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta = (AllowPrivateAccess = "true"))
	USceneComponent* HookPoint;

public:	
	// Sets default values for this actor's properties
	ACrane();

	void InitializeCrane(USplineComponent* InSpline);

	void SetOwnerIncenerator(AIncenerator* InIncenerator) { OwnerIncenerator = InIncenerator; }

	UFUNCTION(BlueprintCallable, Category = "Crane")
	void StartSequence();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = "Crane")
	float OpenAngle = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane")
	float MaxHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane")
	float MinHeight = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane")
	float MovementSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crane")
	float DescentSpeed = 150.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Movement")
	class USplineComponent* TargetSpline;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crane")
	ECraneState CurrentState = ECraneState::Idle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crane")
	float DistanceAlongSpline = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crane")
	float CurrentHeight = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crane")
	AActor* HookedAnomaly = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Crane")
	AIncenerator* OwnerIncenerator = nullptr;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void StartDescending();
	void StartAscending();

private:
	void HandleMovingForward(float DeltaTime);
	void HandleDescending(float DeltaTime);
	void HandleHooking();
	void HandleAscending(float DeltaTime);
	void HandleMovingBackward(float DeltaTime);
	void HandleDropping(float DeltaTime);
};
