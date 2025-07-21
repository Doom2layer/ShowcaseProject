// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/WeaponHud/WeaponHUD.h"

#include "Components/Image.h"
#include "Animation/WidgetAnimation.h"
#include "Components/TextBlock.h"

void UWeaponHUD::NativeConstruct()
{
	Super::NativeConstruct();
	// Start hidden
	SetVisibility(ESlateVisibility::Collapsed);
	bIsVisible = false;
	bIsAiming = false;
	bIsFiring = false;
}

void UWeaponHUD::UpdateWeaponInfo(UTexture2D* WeaponIcon, int32 CurrentAmmo, int32 TotalAmmo)
{
	if (WeaponIconImage && WeaponIcon)
	{
		WeaponIconImage->SetBrushFromTexture(WeaponIcon);
	}
	if (CurrentAmmoText)
	{
		CurrentAmmoText->SetText(FText::AsNumber(CurrentAmmo));
	}
	if (TotalAmmoText)
	{
		TotalAmmoText->SetText(FText::AsNumber(TotalAmmo));
	}
	if (AmmoSeparatorText)
	{
		AmmoSeparatorText->SetText(FText::FromString(TEXT("/")));
	}
}

void UWeaponHUD::StartAiming()
{
	UE_LOG(LogTemp, Log, TEXT("WeaponHUD: Start Aiming"));
	bIsAiming = true;
    
	// Clear any auto-hide timer since we're aiming
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(FireAutoHideTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(FireAutoHideTimer);
	}
    
	if (!bIsVisible)
	{
		ShowWithAnimation();
	}
}

void UWeaponHUD::StopAiming()
{
	UE_LOG(LogTemp, Log, TEXT("WeaponHUD: Stop Aiming"));
	bIsAiming = false;
    
	// Only hide if we're not currently firing
	if (!bIsFiring && bIsVisible)
	{
		HideWithAnimation();
	}
}

void UWeaponHUD::OnWeaponFired()
{
	UE_LOG(LogTemp, Log, TEXT("WeaponHUD: Weapon Fired"));
	bIsFiring = true;
    
	// Clear any existing auto-hide timer
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(FireAutoHideTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(FireAutoHideTimer);
	}
    
	// Show immediately if not visible
	if (!bIsVisible)
	{
		ShowWithAnimation();
	}
}

void UWeaponHUD::OnWeaponStoppedFiring()
{
	UE_LOG(LogTemp, Log, TEXT("WeaponHUD: Weapon Stopped Firing"));
	bIsFiring = false;
    
	// Only start auto-hide if not aiming
	if (!bIsAiming && bIsVisible)
	{
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(
				FireAutoHideTimer,
				this,
				&UWeaponHUD::AutoHideAfterFiring,
				FireAutoHideDelay,
				false
			);
		}
	}
}

void UWeaponHUD::ForceHide()
{
	UE_LOG(LogTemp, Log, TEXT("WeaponHUD: Force Hide"));
	bIsAiming = false;
	bIsFiring = false;
    
	// Clear any timers
	if (GetWorld() && GetWorld()->GetTimerManager().IsTimerActive(FireAutoHideTimer))
	{
		GetWorld()->GetTimerManager().ClearTimer(FireAutoHideTimer);
	}
    
	if (bIsVisible)
	{
		HideWithAnimation();
	}
}

void UWeaponHUD::UpdateCrosshair(UTexture2D* CrosshairTexture, FLinearColor CrosshairColor, float CrosshairSize)
{
	if (CrosshairImage && CrosshairTexture)
	{
		CrosshairImage->SetBrushFromTexture(CrosshairTexture);
		CrosshairImage->SetColorAndOpacity(CrosshairColor);
        
		// Scale the crosshair
		CrosshairImage->SetRenderScale(FVector2D(CrosshairSize, CrosshairSize));
	}
}

void UWeaponHUD::ShowCrosshair()
{
	if (CrosshairImage)
	{
		CrosshairImage->SetVisibility(ESlateVisibility::Visible);
	}
}

void UWeaponHUD::HideCrosshair()
{
	if (CrosshairImage)
	{
		CrosshairImage->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UWeaponHUD::ShowWithAnimation()
{
	UE_LOG(LogTemp, Log, TEXT("WeaponHUD: Showing with animation"));
	bIsVisible = true;
    
	// Stop any fade out animation
	if (FadeOutAnimation && IsAnimationPlaying(FadeOutAnimation))
	{
		StopAnimation(FadeOutAnimation);
	}
    
	SetVisibility(ESlateVisibility::Visible);
    
	if (FadeInAnimation)
	{
		PlayAnimation(FadeInAnimation);
	}
}

void UWeaponHUD::HideWithAnimation()
{
	UE_LOG(LogTemp, Log, TEXT("WeaponHUD: Hiding with animation"));
    
	// Stop any fade in animation
	if (FadeInAnimation && IsAnimationPlaying(FadeInAnimation))
	{
		StopAnimation(FadeInAnimation);
	}
    
	if (FadeOutAnimation)
	{
		PlayAnimation(FadeOutAnimation);
        
		// Set timer to collapse after animation
		if (GetWorld())
		{
			float AnimationLength = FadeOutAnimation->GetEndTime();
			FTimerHandle FadeCompleteTimer;
			GetWorld()->GetTimerManager().SetTimer(
				FadeCompleteTimer,
				this,
				&UWeaponHUD::OnFadeOutComplete,
				AnimationLength,
				false
			);
		}
	}
	else
	{
		OnFadeOutComplete();
	}
}

void UWeaponHUD::OnFadeOutComplete()
{
	bIsVisible = false;
	SetVisibility(ESlateVisibility::Collapsed);
}

void UWeaponHUD::AutoHideAfterFiring()
{
	UE_LOG(LogTemp, Log, TEXT("WeaponHUD: Auto-hiding after firing delay"));
	if (!bIsAiming && !bIsFiring && bIsVisible)
	{
		HideWithAnimation();
	}
}