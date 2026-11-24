// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TabletComponent.generated.h"

class UAnomalyData;
class UTableUI;
class UUserWidget;
class UWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnTabletSheetUpdated);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabletUpdateAnomaly, bool, bHasSheet);

UCLASS(ClassGroup = (Tablet), meta = (BlueprintSpawnableComponent))
class INLANDTOLL_API UTabletComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Tablet")
	FOnTabletSheetUpdated OnTabletSheetUpdated;

	UPROPERTY(BlueprintAssignable, Category = "Tablet")
	FOnTabletUpdateAnomaly OnTabletUpdateAnomaly;

	void InitializeWidgets(UTableUI* SheetWidget, UUserWidget* DefaultWidget, UWidget* TabletContainer);
	void SetOpen(bool bOpen);
	bool AreWidgetsReady() const;
	bool UpdateAnomaly(const UAnomalyData* CurrentInspectionData);
	void RemoveSheet();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tablet")
	bool bHasSheet = false;

	UFUNCTION(BlueprintCallable, Category = "Tablet")
	void NotifyTabletSheetUpdated();

	bool CanBeDelivered() const;
	bool HasSheet() const;

	UFUNCTION(BlueprintCallable, Category = "Tablet")
	int32 GetSheetScore() const;

	UFUNCTION(BlueprintCallable, Category = "Tablet")
	UTableUI* GetTabletWidget() const;

private:
	void UpdateWidgetVisibility();

	UPROPERTY(Transient)
	TObjectPtr<UTableUI> TabletWidget;

	UPROPERTY(Transient)
	TObjectPtr<UUserWidget> DefaultPageWidget;

	UPROPERTY(Transient)
	TObjectPtr<UWidget> TabletContainerWidget;

	bool bIsOpen = false;
};
