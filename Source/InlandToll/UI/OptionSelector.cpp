#include "OptionSelector.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"

void UOptionSelector::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (PreviousButton)
	{
		PreviousButton->OnClicked.AddDynamic(this, &UOptionSelector::HandlePrevious);
	}
	if (NextButton)
	{
		NextButton->OnClicked.AddDynamic(this, &UOptionSelector::HandleNext);
	}
}

void UOptionSelector::NativePreConstruct()
{
	Super::NativePreConstruct();
	UpdateDisplay();
}

void UOptionSelector::SetOptions(const TArray<FText>& InOptions, int32 InSelectedIndex)
{
	Options = InOptions;
	SelectedIndex = Options.IsEmpty() ? 0 : FMath::Clamp(InSelectedIndex, 0, Options.Num() - 1);
	UpdateDisplay();
}

void UOptionSelector::SetSelectedIndex(int32 NewIndex, bool bNotify)
{
	if (Options.IsEmpty())
	{
		return;
	}
	NewIndex = FMath::Clamp(NewIndex, 0, Options.Num() - 1);
	const bool bChanged = NewIndex != SelectedIndex;
	SelectedIndex = NewIndex;
	UpdateDisplay();
	if (bNotify && bChanged)
	{
		OnSelectionChanged.Broadcast(SelectedIndex);
	}
}

void UOptionSelector::SelectNext()
{
	const int32 Num = Options.Num();
	if (Num == 0)
	{
		return;
	}
	const int32 Next = bWrapAround ? (SelectedIndex + 1) % Num : FMath::Min(SelectedIndex + 1, Num - 1);
	SetSelectedIndex(Next, true);
}

void UOptionSelector::SelectPrevious()
{
	const int32 Num = Options.Num();
	if (Num == 0)
	{
		return;
	}
	const int32 Prev = bWrapAround ? (SelectedIndex - 1 + Num) % Num : FMath::Max(SelectedIndex - 1, 0);
	SetSelectedIndex(Prev, true);
}

void UOptionSelector::UpdateDisplay()
{
	if (ValueText)
	{
		ValueText->SetText(Options.IsValidIndex(SelectedIndex) ? Options[SelectedIndex] : FText::GetEmpty());
	}
	if (LabelText)
	{
		LabelText->SetText(Label);
	}
}
