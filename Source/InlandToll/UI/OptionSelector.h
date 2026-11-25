#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionSelector.generated.h"

class UButton;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnOptionSelectionChanged, int32, NewIndex);

// Parent class for WBP_OptionSelector: "< Value >" textual selector.
UCLASS()
class INLANDTOLL_API UOptionSelector : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "OptionSelector")
	void SetOptions(const TArray<FText>& InOptions, int32 InSelectedIndex = 0);

	UFUNCTION(BlueprintCallable, Category = "OptionSelector")
	void SetSelectedIndex(int32 NewIndex, bool bNotify = false);

	UFUNCTION(BlueprintPure, Category = "OptionSelector")
	int32 GetSelectedIndex() const { return SelectedIndex; }

	UFUNCTION(BlueprintCallable, Category = "OptionSelector")
	void SelectNext();

	UFUNCTION(BlueprintCallable, Category = "OptionSelector")
	void SelectPrevious();

	UPROPERTY(BlueprintAssignable, Category = "OptionSelector")
	FOnOptionSelectionChanged OnSelectionChanged;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativePreConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> PreviousButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> NextButton;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ValueText;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> LabelText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OptionSelector")
	FText Label;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OptionSelector")
	TArray<FText> Options;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "OptionSelector")
	bool bWrapAround = true;

private:
	UFUNCTION()
	void HandlePrevious() { SelectPrevious(); }

	UFUNCTION()
	void HandleNext() { SelectNext(); }

	void UpdateDisplay();

	int32 SelectedIndex = 0;
};
