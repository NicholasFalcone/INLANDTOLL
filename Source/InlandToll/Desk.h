#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AnomalyData.h"
#include "InspectionDayDataAsset.h"
#include "Desk.generated.h"

class ATablet;
class UAnomalyData;
class UWidgetComponent;
class UChildActorComponent;
class ABaseInteractable;
class AInspectionManager;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDailyInspectionLimitReached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnErrorCountChanged, int32, NewErrorCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMaxErrorsReached);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDailyInspectionCountChanged, int32, NewDailyInspectionCount);

UCLASS()
class INLANDTOLL_API ADesk : public AActor
{
    GENERATED_BODY()

public:
    ADesk();

private:
    const UAnomalyData* CurrentAnomalyDetails;
    ATablet* TabletInstance;
    
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

    UFUNCTION()
    void OnNewAnomalyButtonPressed();

    UFUNCTION()
    void OnInspectionEnded();

    UFUNCTION()
    void OnInspectionErrorIncreese();

    UFUNCTION()
    void UpdateTabletDetails(const UAnomalyData* NewAnomalyDetails);

    UFUNCTION()
    void DeliverAnomalySheet();

    UAnomalyData* GetInspectionDataForCurrentDay(int32 inspectionIndex);

    UFUNCTION()
    bool IsDayOver(int32 inspectionIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Desk Functions")
	int32 GetInspectionToDailyReach() const{
        return InspectionDataArray[CurrentDayReach].InspectionData.Num();
    }

    UFUNCTION(BlueprintCallable, Category = "Desk Functions")
    int LoadNextDay();

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inspection Car Manager")
	TArray<FInspectionDayDataAsset> InspectionDataArray;

    // Child Actor Components per i pulsanti
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
    TObjectPtr<UChildActorComponent> PrintDetailsButtonComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
    TObjectPtr<UChildActorComponent> ApproveButtonComponent;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
    TObjectPtr<UChildActorComponent> RejectButtonComponent;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Interactable")
    TObjectPtr<UChildActorComponent> NewAnomalyButtonComponent;

    // Monitor UI
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UWidgetComponent> LeftMonitor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
    TObjectPtr<UWidgetComponent> RightMonitor;

	UPROPERTY(BlueprintAssignable, Category = "Custom Event")
    FOnDailyInspectionLimitReached OnDailyInspectionLimitReached;
    UPROPERTY(BlueprintAssignable, Category = "Custom Event")
    FOnErrorCountChanged OnErrorCountChanged;
    UPROPERTY(BlueprintAssignable, Category = "Custom Event")
    FOnMaxErrorsReached OnMaxErrorsReached;
    UPROPERTY(BlueprintAssignable, Category = "Custom Event")
    FOnDailyInspectionCountChanged OnDailyInspectionCountChanged;

public:
    // Helper function se vuoi accedere comodamente alla classe ABaseInteractable nel codice
    ABaseInteractable* GetApproveButton() const;
    ABaseInteractable* GetRejectButton() const;
    ABaseInteractable* GetPrintDetailsButton() const;
    ABaseInteractable* GetNewAnomalyButtonComponent() const;

    ATablet* GetTabletInstance();


    AInspectionManager* InspectionManager;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Desk Functions")
	int32 MaxErrorsAllowed = 3;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Desk Functions")
	int32 CurrentErrors = 0;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Desk Functions")
    int32 CurrentDayReach = 0;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Desk Functions")
	int32 InspectionPassed = 0;


};