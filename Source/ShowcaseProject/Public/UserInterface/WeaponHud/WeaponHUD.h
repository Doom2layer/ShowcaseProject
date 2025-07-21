// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WeaponHUD.generated.h"

class UImage;
class UTextBlock;
class UWidgetAnimation;

/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UWeaponHUD : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintCallable)
	void UpdateWeaponInfo(UTexture2D* WeaponIcon, int32 CurrentAmmo, int32 TotalAmmo);

	// New aiming functions
	UFUNCTION(BlueprintCallable)
	void StartAiming();

	UFUNCTION(BlueprintCallable)
	void StopAiming();

	// New firing functions
	UFUNCTION(BlueprintCallable)
	void OnWeaponFired();

	UFUNCTION(BlueprintCallable)
	void OnWeaponStoppedFiring();

	// Force hide (for weapon unequip)
	UFUNCTION(BlueprintCallable)
	void ForceHide();

	UPROPERTY(meta = (BindWidget))
	UImage* CrosshairImage;

	UFUNCTION(BlueprintCallable)
	void UpdateCrosshair(UTexture2D* CrosshairTexture, FLinearColor CrosshairColor, float CrosshairSize);

	UFUNCTION(BlueprintCallable)
	void ShowCrosshair();

	UFUNCTION(BlueprintCallable)
	void HideCrosshair();
	
protected:
	UPROPERTY(meta = (BindWidget))
	UImage* WeaponIconImage;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CurrentAmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TotalAmmoText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* AmmoSeparatorText;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeInAnimation;

	UPROPERTY(Transient, meta = (BindWidgetAnim))
	UWidgetAnimation* FadeOutAnimation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Auto Hide")
	float FireAutoHideDelay = 5.0f;

private:
	FTimerHandle FireAutoHideTimer;
    
	bool bIsAiming = false;
	bool bIsFiring = false;
	bool bIsVisible = false;

	void ShowWithAnimation();
	void HideWithAnimation();
	void OnFadeOutComplete();

	UFUNCTION()
	void AutoHideAfterFiring();
};
