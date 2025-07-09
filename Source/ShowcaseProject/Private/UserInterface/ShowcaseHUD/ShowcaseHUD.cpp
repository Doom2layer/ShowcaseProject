// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/ShowcaseHUD/ShowcaseHUD.h"

#include "Kismet/GameplayStatics.h"
#include "UserInterface/MainMenu/MainMenu.h"
#include "UserInterface/InventoryMenu/InventoryMenu.h"
#include "UserInterface/Interaction/InteractionWidget.h"


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
	if (InventoryMenuClass)
	{
		InventoryMenuWidget = CreateWidget<UInventoryMenu>(GetWorld(), InventoryMenuClass);
		InventoryMenuWidget->AddToViewport(1);
		InventoryMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
	
	}
	if (InteractionWidgetClass)
	{
		InteractionWidget = CreateWidget<UInteractionWidget>(GetWorld(), InteractionWidgetClass);
		InteractionWidget->AddToViewport(2);
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
			UE_LOG(LogTemp, Warning, TEXT("Main Menu Collapsed"));
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

