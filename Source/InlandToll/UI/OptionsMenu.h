#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "OptionsMenu.generated.h"

class USlider;
class UButton;
class USoundClass;
class UOptionSelector;

// Parent class for WBP_Options: audio sliders + graphics quality / resolution selectors.
UCLASS()
class INLANDTOLL_API UOptionsMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Options")
	void SetSFXVolume(float Value);

	UFUNCTION(BlueprintCallable, Category = "Options")
	void SetAmbientVolume(float Value);

	UFUNCTION(BlueprintCallable, Category = "Options")
	void SetVoiceVolume(float Value);

	UFUNCTION(BlueprintCallable, Category = "Options")
	void SetQualityLevel(int32 Index);

	UFUNCTION(BlueprintCallable, Category = "Options")
	void SetResolutionIndex(int32 Index);

	// Writes volumes to GameUserSettings.ini and applies/saves graphics settings.
	UFUNCTION(BlueprintCallable, Category = "Options")
	void ApplyAndSave();

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> SFXSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> AmbientSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> VoiceSlider;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> QualitySelector;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOptionSelector> ResolutionSelector;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> ApplyButton;

	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UButton> BackButton;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Options|Audio")
	TObjectPtr<USoundClass> SFXSoundClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Options|Audio")
	TObjectPtr<USoundClass> AmbientSoundClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Options|Audio")
	TObjectPtr<USoundClass> VoiceSoundClass;

	// Implement in WBP_Options to close/hide the panel.
	UFUNCTION(BlueprintImplementableEvent, Category = "Options")
	void OnBackRequested();

private:
	UFUNCTION()
	void HandleSFXChanged(float Value) { SetSFXVolume(Value); }
	UFUNCTION()
	void HandleAmbientChanged(float Value) { SetAmbientVolume(Value); }
	UFUNCTION()
	void HandleVoiceChanged(float Value) { SetVoiceVolume(Value); }
	UFUNCTION()
	void HandleQualityChanged(int32 Index) { SetQualityLevel(Index); }
	UFUNCTION()
	void HandleResolutionChanged(int32 Index) { SetResolutionIndex(Index); }
	UFUNCTION()
	void HandleApply() { ApplyAndSave(); }
	UFUNCTION()
	void HandleBack() { ApplyAndSave(); OnBackRequested(); }

	static float GetClassVolume(const USoundClass* SoundClass);
	static void SetClassVolume(USoundClass* SoundClass, float Value);
	void LoadVolume(USoundClass* SoundClass, const TCHAR* Key, USlider* Slider) const;

	TArray<FIntPoint> Resolutions;
};
