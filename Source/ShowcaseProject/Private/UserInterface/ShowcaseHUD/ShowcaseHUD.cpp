// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/ShowcaseHUD/ShowcaseHUD.h"

#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "Player/ShowcaseProjectCharacter.h"
#include "UserInterface/MainMenu/MainMenu.h"
#include "Weapons/WeaponBase.h"
#include "UserInterface/WeaponHud/WeaponHUD.h"
#include "UserInterface/InventoryMenu/InventoryMenu.h"
#include "UserInterface/Interaction/InteractionWidget.h"
#include "UserInterface/Inventory/InventoryPanel.h"


AShowcaseHUD::AShowcaseHUD()
{
}

void AShowcaseHUD::BeginPlay()
{
	Super::BeginPlay();

	if (MainMenuWidgetClass)
	{
		MainMenuWidget = CreateWidget<UMainMenu>(GetWorld(), MainMenuWidgetClass);
		MainMenuWidget->AddToViewport(0);
		MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (WeaponHUDClass)
	{
		WeaponHUDWidget = CreateWidget<UWeaponHUD>(GetWorld(), WeaponHUDClass);
		WeaponHUDWidget->AddToViewport(1);
		WeaponHUDWidget->SetVisibility(ESlateVisibility::Collapsed);
		// Set position to lower right corner
		if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(WeaponHUDWidget->Slot))
		{
			CanvasSlot->SetAnchors(FAnchors(1.0f, 1.0f, 1.0f, 1.0f));
			CanvasSlot->SetPosition(FVector2D(-250.0f, -120.0f)); 
			CanvasSlot->SetSize(FVector2D(200.0f, 100.0f));
		}
	}
	if (InventoryMenuClass)
	{
		InventoryMenuWidget = CreateWidget<UInventoryMenu>(GetWorld(), InventoryMenuClass);
		InventoryMenuWidget->AddToViewport(2);
		InventoryMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(3);
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AShowcaseHUD::ToggleMainMenu()
{
	if (MainMenuWidget)
	{
		bIsMainMenuVisible = !bIsMainMenuVisible;
		if (bIsMainMenuVisible)
		{
			MainMenuWidget->SetVisibility(ESlateVisibility::Visible);

			const FInputModeUIOnly InputMode;
			GetOwningPlayerController()->SetInputMode(InputMode);
			GetOwningPlayerController()->SetShowMouseCursor(true);
		}
		else
		{
			MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
			const FInputModeGameOnly InputMode;
			GetOwningPlayerController()->SetInputMode(InputMode);
			GetOwningPlayerController()->SetShowMouseCursor(false);
		}
	}
}

void AShowcaseHUD::ToggleInventoryMenu()
{
	if (InventoryMenuWidget)
	{
		bIsInventoryMenuVisible = !bIsInventoryMenuVisible;

		if (bIsInventoryMenuVisible)
		{
			InventoryMenuWidget->SetVisibility(ESlateVisibility::Visible);

			const FInputModeGameAndUI InputMode;
			GetOwningPlayerController()->SetInputMode(InputMode);
			GetOwningPlayerController()->SetShowMouseCursor(true);

			// Slowly lerp to 0.1
			TargetTimeDilation = 0.1f;
		}
		else
		{
			InventoryMenuWidget->SetVisibility(ESlateVisibility::Collapsed);

			const FInputModeGameOnly InputMode;
			GetOwningPlayerController()->SetInputMode(InputMode);
			GetOwningPlayerController()->SetShowMouseCursor(false);

			// Instantly return to normal speed
			TargetTimeDilation = 1.0f;
			CurrentTimeDilation = 1.0f;

			InventoryMenuWidget->WBP_InventoryPanel->CloseActiveContextMenu();
		}
		// Start updating time dilation each tick
		GetWorld()->GetTimerManager().SetTimer(TimeDilationTimerHandle, this, &AShowcaseHUD::UpdateTimeDilation, 0.01f, true);
	}
}

void AShowcaseHUD::UpdateTimeDilation()
{
	CurrentTimeDilation = FMath::FInterpTo(CurrentTimeDilation, TargetTimeDilation, GetWorld()->GetDeltaSeconds(), TimeDilationInterpSpeed);
	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), CurrentTimeDilation);

	if (FMath::IsNearlyEqual(CurrentTimeDilation, TargetTimeDilation, 0.01f))
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TargetTimeDilation);
		GetWorld()->GetTimerManager().ClearTimer(TimeDilationTimerHandle);
	}
}

void AShowcaseHUD::ShowInteractionWidget() const
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void AShowcaseHUD::HideInteractionWidget() const
{
	if (InteractionWidget)
	{
		InteractionWidget->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void AShowcaseHUD::UpdateInteractionWidget(const FInteractableData* InteractableData) const
{
	if (InteractionWidget)
	{
		if (InteractionWidget->GetVisibility() == ESlateVisibility::Collapsed)
		{
			InteractionWidget->SetVisibility(ESlateVisibility::Visible);
		}
		InteractionWidget->UpdateWidget(InteractableData);
	}
}

void AShowcaseHUD::UpdateWeaponDisplay(AWeaponBase* EquippedWeapon)
{
	UE_LOG(LogTemp, Log, TEXT("Updating weapon display for: %s"), *GetNameSafe(EquippedWeapon));
	if (!WeaponHUDWidget) return;

	if (EquippedWeapon && EquippedWeapon->GetWeaponItemData())
	{
        
		WeaponHUDWidget->UpdateWeaponInfo(EquippedWeapon->GetWeaponItemData()->ItemAssetData.Icon,
			EquippedWeapon->GetCurrentAmmoInMagazine(),
			EquippedWeapon->GetCurrentReserveAmmo()
		);
		
		WeaponHUDWidget->UpdateCrosshair(
			EquippedWeapon->GetWeaponItemData()->WeaponData.CrosshairTexture,
			EquippedWeapon->GetWeaponItemData()->WeaponData.CrosshairColor,
			EquippedWeapon->GetWeaponItemData()->WeaponData.CrosshairSize
		);
		WeaponHUDWidget->ShowCrosshair();

		if (GetOwningPlayerController())
		{
			AShowcaseProjectCharacter* Character = Cast<AShowcaseProjectCharacter>(GetOwningPlayerController()->GetPawn());
			if (Character && Character->IsAiming())
			{
				WeaponHUDWidget->StartAiming();
			}
		}
	}
	else
	{
		WeaponHUDWidget->HideCrosshair();
		WeaponHUDWidget->ForceHide();
	}
}

void AShowcaseHUD::OnWeaponAimStart()
{
	if (WeaponHUDWidget)
	{
		WeaponHUDWidget->StartAiming();
	}
}

void AShowcaseHUD::OnWeaponAimStop()
{
	if (WeaponHUDWidget)
	{
		WeaponHUDWidget->StopAiming();
	}
}

void AShowcaseHUD::OnWeaponFired()
{
	if (WeaponHUDWidget)
	{
		WeaponHUDWidget->OnWeaponFired();
	}
}

void AShowcaseHUD::OnWeaponStoppedFiring()
{
	if (WeaponHUDWidget)
	{
		WeaponHUDWidget->OnWeaponStoppedFiring();
	}
}