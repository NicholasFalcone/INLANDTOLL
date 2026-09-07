// Fill out your copyright notice in the Description page of Project Settings.


#include "Crane.h"
#include "Kismet/GameplayStatics.h"
#include "InspectionManager.h"
#include "InspectionPayload.h"
#include "InspectionProp.h"
#include "Incenerator.h"

// Sets default values
ACrane::ACrane()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Rig = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("First Person Mesh"));
	RootComponent = Rig;

	HookPoint = CreateDefaultSubobject<USceneComponent>(TEXT("HookPoint"));
	HookPoint->SetupAttachment(RootComponent);

	MaxHeight = 0.0f;
	MinHeight = -400.0f;
	MovementSpeed = 300.0f;
	DescentSpeed = 150.0f;
	OpenAngle = 45.0f;
	CurrentState = ECraneState::Idle;
	DistanceAlongSpline = 0.0f;
	CurrentHeight = 0.0f;
}

// Called when the game starts or when spawned
void ACrane::BeginPlay()
{
	Super::BeginPlay();
	
	CurrentHeight = MaxHeight;
	if (HookPoint)
	{
		HookPoint->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentHeight));
	}
}

// Called every frame
void ACrane::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	switch (CurrentState)
	{
		case ECraneState::Idle:
			break;

		case ECraneState::MovingForward:
			HandleMovingForward(DeltaTime);
			break;

		case ECraneState::Descending:
			HandleDescending(DeltaTime);
			break;

		case ECraneState::Hooking:
			HandleHooking();
			break;

		case ECraneState::Ascending:
			HandleAscending(DeltaTime);
			break;

		case ECraneState::MovingBackward:
			HandleMovingBackward(DeltaTime);
			break;

		case ECraneState::Dropping:
			HandleDropping(DeltaTime);
			break;
	}
}

void ACrane::StartDescending()
{
	CurrentState = ECraneState::Descending;
}

void ACrane::StartAscending()
{
	CurrentState = ECraneState::Ascending;
}

void ACrane::InitializeCrane(USplineComponent* InSpline)
{
	TargetSpline = InSpline;

	if(TargetSpline == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("TargetSpline is null in InitializeCrane!"));
		return;
	}

	SetActorLocationAndRotation(TargetSpline->GetLocationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World),
		TargetSpline->GetRotationAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World));
}

void ACrane::StartSequence()
{
	if (CurrentState == ECraneState::Idle)
	{
		UE_LOG(LogTemp, Warning, TEXT("Crane beginning sequence..."));
		DistanceAlongSpline = 0.0f;
		CurrentHeight = MaxHeight;
		if (HookPoint)
		{
			HookPoint->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentHeight));
		}
		OpenAngle = 45.0f; // Claws open
		HookedAnomaly = nullptr;
		CurrentState = ECraneState::MovingForward;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Crane is already in sequence or busy (State: %d)"), (int32)CurrentState);
	}
}

void ACrane::HandleMovingForward(float DeltaTime)
{
	if (!TargetSpline)
	{
		UE_LOG(LogTemp, Error, TEXT("No TargetSpline assigned during MovingForward!"));
		CurrentState = ECraneState::Idle;
		return;
	}

	float SplineLength = TargetSpline->GetSplineLength();
	DistanceAlongSpline += MovementSpeed * DeltaTime;

	if (DistanceAlongSpline >= SplineLength)
	{
		DistanceAlongSpline = SplineLength;
		SetActorLocationAndRotation(
			TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World),
			TargetSpline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World)
		);
		
		UE_LOG(LogTemp, Warning, TEXT("Crane reached end of spline. Starting descent."));
		StartDescending();
	}
	else
	{
		SetActorLocationAndRotation(
			TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World),
			TargetSpline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World)
		);
	}
}

void ACrane::HandleDescending(float DeltaTime)
{
	CurrentHeight = FMath::FInterpConstantTo(CurrentHeight, MinHeight, DeltaTime, DescentSpeed);
	if (HookPoint)
	{
		HookPoint->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentHeight));
	}

	if (FMath::IsNearlyEqual(CurrentHeight, MinHeight, 1.0f))
	{
		CurrentHeight = MinHeight;
		if (HookPoint)
		{
			HookPoint->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentHeight));
		}
		UE_LOG(LogTemp, Warning, TEXT("Crane reached bottom height. Hooking anomaly..."));
		CurrentState = ECraneState::Hooking;
	}
}

void ACrane::HandleHooking()
{
	AActor* FoundAnomaly = nullptr;

	// Method 1: Check from current inspection car via InspectionManager
	if (AInspectionManager* Mgr = Cast<AInspectionManager>(UGameplayStatics::GetActorOfClass(GetWorld(), AInspectionManager::StaticClass())))
	{
		if (Mgr->CurrentInspectionCar)
		{
			TArray<AActor*> AttachedActors;
			Mgr->CurrentInspectionCar->GetAttachedActors(AttachedActors);
			for (AActor* Actor : AttachedActors)
			{
				if (Actor && Actor->IsA(AInspectionProp::StaticClass()))
				{
					FoundAnomaly = Actor;
					break;
				}
			}

			// Mark car as rejected and send it on its way!
			Mgr->CurrentInspectionCar->bIsRejected = true;
			Mgr->CurrentInspectionCar->ResumeMovementToEnd();
			Mgr->CurrentInspectionCar->OnCarReachedEnd.AddDynamic(Mgr, &AInspectionManager::HandleCarReachedEnd);
		}
	}

	// Method 2: Fallback to finding the nearest AInspectionProp in range
	if (!FoundAnomaly)
	{
		TArray<AActor*> AllProps;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AInspectionProp::StaticClass(), AllProps);
		float BestDist = 500.0f; // maximum search radius
		for (AActor* Prop : AllProps)
		{
			if (Prop)
			{
				float Dist = FVector::Dist(Prop->GetActorLocation(), HookPoint->GetComponentLocation());
				if (Dist < BestDist)
				{
					BestDist = Dist;
					FoundAnomaly = Prop;
				}
			}
		}
	}

	if (FoundAnomaly)
	{
		UE_LOG(LogTemp, Warning, TEXT("Hooked anomaly: %s"), *FoundAnomaly->GetName());
		HookedAnomaly = FoundAnomaly;

		// 1. Detach from car or any previous parent
		HookedAnomaly->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// 2. Disable physics and collision so it is cleanly carried
		if (UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(HookedAnomaly->GetRootComponent()))
		{
			RootPrimitive->SetSimulatePhysics(false);
		}
		HookedAnomaly->SetActorEnableCollision(false);

		// 3. Attach directly to HookPoint so it moves with the hook
		HookedAnomaly->AttachToComponent(HookPoint, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No anomaly found in range to hook!"));
	}

	// Close the claws
	OpenAngle = 0.0f;

	// Transition to ascending
	StartAscending();
}

void ACrane::HandleAscending(float DeltaTime)
{
	CurrentHeight = FMath::FInterpConstantTo(CurrentHeight, MaxHeight, DeltaTime, DescentSpeed);
	if (HookPoint)
	{
		HookPoint->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentHeight));
	}

	if (FMath::IsNearlyEqual(CurrentHeight, MaxHeight, 1.0f))
	{
		CurrentHeight = MaxHeight;
		if (HookPoint)
		{
			HookPoint->SetRelativeLocation(FVector(0.0f, 0.0f, CurrentHeight));
		}
		UE_LOG(LogTemp, Warning, TEXT("Crane hook retracted to top. Reversing along spline..."));
		CurrentState = ECraneState::MovingBackward;
	}
}

void ACrane::HandleMovingBackward(float DeltaTime)
{
	if (!TargetSpline)
	{
		UE_LOG(LogTemp, Error, TEXT("No TargetSpline assigned during MovingBackward!"));
		CurrentState = ECraneState::Idle;
		return;
	}

	DistanceAlongSpline -= MovementSpeed * DeltaTime;

	if (DistanceAlongSpline <= 0.0f)
	{
		DistanceAlongSpline = 0.0f;
		SetActorLocationAndRotation(
			TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World),
			TargetSpline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World)
		);

		UE_LOG(LogTemp, Warning, TEXT("Crane returned to start of spline (incinerator). Dropping anomaly..."));
		CurrentState = ECraneState::Dropping;
	}
	else
	{
		SetActorLocationAndRotation(
			TargetSpline->GetLocationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World),
			TargetSpline->GetRotationAtDistanceAlongSpline(DistanceAlongSpline, ESplineCoordinateSpace::World)
		);
	}
}

void ACrane::HandleDropping(float DeltaTime)
{
	// Claws open
	OpenAngle = 45.0f;

	if (HookedAnomaly)
	{
		UE_LOG(LogTemp, Warning, TEXT("Dropping anomaly: %s into incinerator"), *HookedAnomaly->GetName());

		// 1. Detach from hook
		HookedAnomaly->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// 2. Re-enable physics and collision so it falls under gravity into the incinerator
		HookedAnomaly->SetActorEnableCollision(true);
		if (UPrimitiveComponent* RootPrimitive = Cast<UPrimitiveComponent>(HookedAnomaly->GetRootComponent()))
		{
			RootPrimitive->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			RootPrimitive->SetSimulatePhysics(true);
		}

		// 3. Notify owner incinerator to play fire effects and destroy the anomaly
		if (OwnerIncenerator)
		{
			OwnerIncenerator->NotifyAnomalyDropped(HookedAnomaly);
		}
		else
		{
			// Fallback: if no incinerator, destroy it ourselves immediately
			HookedAnomaly->Destroy();
		}

		HookedAnomaly = nullptr;
	}

	CurrentState = ECraneState::Idle;
}

