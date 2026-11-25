#include "OptionsMenu.h"
#include "OptionSelector.h"
#include "Components/Slider.h"
#include "Components/Button.h"
#include "Sound/SoundClass.h"
#include "GameFramework/GameUserSettings.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Misc/ConfigCacheIni.h"

namespace
{
	const TCHAR* AudioSection = TEXT("InlandToll.Audio");
	const TCHAR* SFXKey = TEXT("SFXVolume");
	const TCHAR* AmbientKey = TEXT("AmbientVolume");
	const TCHAR* VoiceKey = TEXT("VoiceVolume");
	constexpr int32 QualityLevels = 4;
}

void UOptionsMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (SFXSlider) SFXSlider->OnValueChanged.AddDynamic(this, &UOptionsMenu::HandleSFXChanged);
	if (AmbientSlider) AmbientSlider->OnValueChanged.AddDynamic(this, &UOptionsMenu::HandleAmbientChanged);
	if (VoiceSlider) VoiceSlider->OnValueChanged.AddDynamic(this, &UOptionsMenu::HandleVoiceChanged);
	if (QualitySelector) QualitySelector->OnSelectionChanged.AddDynamic(this, &UOptionsMenu::HandleQualityChanged);
	if (ResolutionSelector) ResolutionSelector->OnSelectionChanged.AddDynamic(this, &UOptionsMenu::HandleResolutionChanged);
	if (ApplyButton) ApplyButton->OnClicked.AddDynamic(this, &UOptionsMenu::HandleApply);
	if (BackButton) BackButton->OnClicked.AddDynamic(this, &UOptionsMenu::HandleBack);
}

void UOptionsMenu::NativeConstruct()
{
	Super::NativeConstruct();

	LoadVolume(SFXSoundClass, SFXKey, SFXSlider);
	LoadVolume(AmbientSoundClass, AmbientKey, AmbientSlider);
	LoadVolume(VoiceSoundClass, VoiceKey, VoiceSlider);

	UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings();
	if (!Settings)
	{
		return;
	}

	if (QualitySelector)
	{
		QualitySelector->SetOptions({
			NSLOCTEXT("Options", "QualityLow", "Low"),
			NSLOCTEXT("Options", "QualityMedium", "Medium"),
			NSLOCTEXT("Options", "QualityHigh", "High"),
			NSLOCTEXT("Options", "QualityEpic", "Epic") },
			FMath::Clamp(Settings->GetOverallScalabilityLevel(), 0, QualityLevels - 1));
	}

	if (ResolutionSelector)
	{
		Resolutions.Reset();
		TArray<FText> Labels;
		UKismetSystemLibrary::GetSupportedFullscreenResolutions(Resolutions);
		const FIntPoint Current = Settings->GetScreenResolution();
		if (!Resolutions.Contains(Current))
		{
			Resolutions.Add(Current);
		}
		Resolutions.Sort([](const FIntPoint& A, const FIntPoint& B) { return A.X != B.X ? A.X < B.X : A.Y < B.Y; });
		for (const FIntPoint& R : Resolutions)
		{
			Labels.Add(FText::FromString(FString::Printf(TEXT("%d x %d"), R.X, R.Y)));
		}
		ResolutionSelector->SetOptions(Labels, Resolutions.IndexOfByKey(Current));
	}
}

float UOptionsMenu::GetClassVolume(const USoundClass* SoundClass)
{
	return SoundClass ? SoundClass->Properties.Volume : 1.f;
}

void UOptionsMenu::SetClassVolume(USoundClass* SoundClass, float Value)
{
	if (SoundClass)
	{
		SoundClass->Properties.Volume = FMath::Clamp(Value, 0.f, 1.f);
	}
}

void UOptionsMenu::LoadVolume(USoundClass* SoundClass, const TCHAR* Key, USlider* Slider) const
{
	float Value = GetClassVolume(SoundClass);
	GConfig->GetFloat(AudioSection, Key, Value, GGameUserSettingsIni);
	SetClassVolume(SoundClass, Value);
	if (Slider)
	{
		Slider->SetValue(Value);
	}
}

void UOptionsMenu::SetSFXVolume(float Value) { SetClassVolume(SFXSoundClass, Value); }
void UOptionsMenu::SetAmbientVolume(float Value) { SetClassVolume(AmbientSoundClass, Value); }
void UOptionsMenu::SetVoiceVolume(float Value) { SetClassVolume(VoiceSoundClass, Value); }

void UOptionsMenu::SetQualityLevel(int32 Index)
{
	if (UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings())
	{
		Settings->SetOverallScalabilityLevel(FMath::Clamp(Index, 0, QualityLevels - 1));
		Settings->ApplySettings(false);
	}
}

void UOptionsMenu::SetResolutionIndex(int32 Index)
{
	if (UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings(); Settings && Resolutions.IsValidIndex(Index))
	{
		Settings->SetScreenResolution(Resolutions[Index]);
		Settings->ApplyResolutionSettings(false);
	}
}

void UOptionsMenu::ApplyAndSave()
{
	if (GConfig)
	{
		GConfig->SetFloat(AudioSection, SFXKey, GetClassVolume(SFXSoundClass), GGameUserSettingsIni);
		GConfig->SetFloat(AudioSection, AmbientKey, GetClassVolume(AmbientSoundClass), GGameUserSettingsIni);
		GConfig->SetFloat(AudioSection, VoiceKey, GetClassVolume(VoiceSoundClass), GGameUserSettingsIni);
		GConfig->Flush(false, GGameUserSettingsIni);
	}
	if (UGameUserSettings* Settings = UGameUserSettings::GetGameUserSettings())
	{
		Settings->ApplySettings(false);
		Settings->SaveSettings();
	}
}
