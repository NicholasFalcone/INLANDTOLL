#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Desk.generated.h"

class UWidgetComponent;
class UChildActorComponent;
class ABaseInteractable;
class AInspectionManager;

UCLASS()
class INLANDTOLL_API ADesk : public AActor
{
    GENERATED_BODY()

public:
    ADesk();

protected:
    virtual void BeginPlay() override;

public:
    UFUNCTION()
    void RestartGame();
    UFUNCTION()
    void SetupButtons();
    UFUNCTION()
    void OnPrintDetailsButtonPressed();
    UFUNCTION()
    void OnApproveButtonPressed();
    UFUNCTION()
    void OnRejectButtonPressed();

public:
    // Child Actor Components per i pulsanti
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
    TObjectPtr<UChildActorComponent> PrintDetailsButtonComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
    TObjectPtr<UChildActorComponent> ApproveButtonComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
    TObjectPtr<UChildActorComponent> RejectButtonComponent;

    // Monitor UI
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UWidgetComponent> LeftMonitor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UWidgetComponent> RightMonitor;

public:
    // Helper function se vuoi accedere comodamente alla classe ABaseInteractable nel codice
    ABaseInteractable* GetApproveButton() const;
    ABaseInteractable* GetRejectButton() const;
    ABaseInteractable* GetPrintDetailsButton() const;

    AInspectionManager* InspectionManager;
};