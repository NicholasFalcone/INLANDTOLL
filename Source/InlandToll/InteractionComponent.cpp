// Fill out your copyright notice in the Description page of Project Settings.


#include "InteractionComponent.h"
#include "HorrorCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/PrimitiveComponent.h"
#include "Engine/OverlapResult.h"
#include "Engine/World.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "Components/BoxComponent.h"
#include "Misc/AutomationTest.h"
#endif

namespace
{
ABaseInteractable* FindInteractableInSphere(UWorld* World, const FVector& Origin, const FVector& Forward,
    float Radius, const FCollisionQueryParams& QueryParams)
{
    TArray<FOverlapResult> Overlaps;
    World->OverlapMultiByObjectType(Overlaps, Origin, FQuat::Identity,
        FCollisionObjectQueryParams(FCollisionObjectQueryParams::AllObjects),
        FCollisionShape::MakeSphere(Radius), QueryParams);

    ABaseInteractable* BestInteractable = nullptr;
    double BestAlignment = 0.0;
    double BestDistanceSquared = TNumericLimits<double>::Max();
    for (const FOverlapResult& Overlap : Overlaps)
    {
        ABaseInteractable* Interactable = Cast<ABaseInteractable>(Overlap.GetActor());
        UPrimitiveComponent* Component = Overlap.GetComponent();
        if (!IsValid(Interactable) || !Interactable->bIsInteractable || !Component)
        {
            continue;
        }

        const FVector Offset = Component->Bounds.Origin - Origin;
        const double DistanceSquared = Offset.SizeSquared();
        const double Alignment = FVector::DotProduct(Forward, Offset.GetSafeNormal());
        if (Alignment > 0.0 && (Alignment > BestAlignment + KINDA_SMALL_NUMBER
            || (FMath::IsNearlyEqual(Alignment, BestAlignment) && DistanceSquared < BestDistanceSquared)))
        {
            BestInteractable = Interactable;
            BestAlignment = Alignment;
            BestDistanceSquared = DistanceSquared;
        }
    }

    return BestInteractable;
}
}

// Sets default values for this component's properties
UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();
    if (!Cast<AHorrorCharacter>(GetOwner()))
    {
        UE_LOG(LogTemp, Error, TEXT("InteractionComponent requires an AHorrorCharacter owner: %s"), *GetNameSafe(GetOwner()));
    }
}

ABaseInteractable* UInteractionComponent::CheckForInteractables()
{
    UWorld* World = GetWorld();
    AHorrorCharacter* OwnerCharacter = Cast<AHorrorCharacter>(GetOwner());
    if (!World || !IsValid(OwnerCharacter) || InteractionDistance <= 0.0f) return nullptr;

    UCameraComponent* Camera = OwnerCharacter->GetFirstPersonCameraComponent();
    if (!Camera) return nullptr;

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(OwnerCharacter);
    QueryParams.AddIgnoredActor(OwnerCharacter->GetEquippedTool());

    return FindInteractableInSphere(World, Camera->GetComponentLocation(), Camera->GetForwardVector(),
        InteractionDistance, QueryParams);
}






// Called every frame
void UInteractionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	ABaseInteractable* Interactable = CheckForInteractables();

    if (Interactable) {
        if(CurrentInteractable != Interactable) {
            
            if(IsValid(CurrentInteractable)) {
                CurrentInteractable->OnUnhighlight();
            }
            CurrentInteractable = Interactable;
            CurrentInteractable->OnHighlight();
		}
    }
    else 
    {
        if (IsValid(CurrentInteractable)) {
            CurrentInteractable->OnUnhighlight();
        }
        CurrentInteractable = nullptr;
    }
}

#if WITH_DEV_AUTOMATION_TESTS
IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInteractionSphereTest, "InlandToll.Interaction.SphereSelection",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

bool FInteractionSphereTest::RunTest(const FString& Parameters)
{
    UWorld* World = UWorld::CreateWorld(EWorldType::Game, false);
    if (!TestNotNull(TEXT("Test world"), World))
    {
        return false;
    }

    FActorSpawnParameters SpawnParameters;
    SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    ABaseInteractable* OffCenter = World->SpawnActor<ABaseInteractable>(
        FVector(100.0, 40.0, 0.0), FRotator::ZeroRotator, SpawnParameters);
    ABaseInteractable* Centered = World->SpawnActor<ABaseInteractable>(
        FVector(150.0, 0.0, 0.0), FRotator::ZeroRotator, SpawnParameters);
    AActor* Obstacle = World->SpawnActor<AActor>(
        FVector(50.0, 0.0, 0.0), FRotator::ZeroRotator, SpawnParameters);
    if (!TestNotNull(TEXT("Off-center interactable"), OffCenter)
        || !TestNotNull(TEXT("Centered interactable"), Centered)
        || !TestNotNull(TEXT("Obstacle"), Obstacle))
    {
        World->DestroyWorld(false);
        return false;
    }

    auto AddCollision = [](AActor* Actor)
    {
        UBoxComponent* Box = NewObject<UBoxComponent>(Actor);
        Box->SetBoxExtent(FVector(10.0));
        Box->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        Box->SetCollisionObjectType(ECC_WorldDynamic);
        Box->SetCollisionResponseToAllChannels(ECR_Block);
        if (Actor->GetRootComponent())
        {
            Box->SetupAttachment(Actor->GetRootComponent());
        }
        else
        {
            Actor->SetRootComponent(Box);
        }
        Box->RegisterComponent();
    };
    AddCollision(OffCenter);
    AddCollision(Centered);
    AddCollision(Obstacle);
    Obstacle->SetActorLocation(FVector(50.0, 0.0, 0.0));

    const FVector Origin = FVector::ZeroVector;
    const FVector Forward = FVector::ForwardVector;
    const FCollisionQueryParams QueryParams;
    auto Find = [&]()
    {
        return FindInteractableInSphere(World, Origin, Forward, 200.0f, QueryParams);
    };

    TestEqual(TEXT("Most centered interactable is selected through an obstacle"), Find(), Centered);
    OffCenter->SetActorLocation(FVector(100.0, 0.0, 0.0));
    TestEqual(TEXT("Nearest interactable wins when both are centered"), Find(), OffCenter);
    OffCenter->SetActorLocation(FVector(100.0, 40.0, 0.0));
    Centered->bIsInteractable = false;
    TestEqual(TEXT("Disabled interactables are ignored"), Find(), OffCenter);
    OffCenter->bIsInteractable = false;
    Centered->bIsInteractable = true;
    Centered->SetActorLocation(FVector(-150.0, 0.0, 0.0));
    TestNull(TEXT("Interactables behind the camera are ignored"), Find());
    Centered->SetActorLocation(FVector(205.0, 0.0, 0.0));
    TestEqual(TEXT("Collision overlapping the sphere is in range"), Find(), Centered);
    Centered->SetActorLocation(FVector(211.0, 0.0, 0.0));
    TestNull(TEXT("Collision outside the sphere is out of range"), Find());

    World->DestroyWorld(false);
    return true;
}
#endif

