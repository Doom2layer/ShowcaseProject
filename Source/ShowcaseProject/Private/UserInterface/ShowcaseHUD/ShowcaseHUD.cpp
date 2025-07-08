// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/ShowcaseHUD/ShowcaseHUD.h"
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
			MainMenuWidget->SetFocus();
		}
		else
		{
			MainMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
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
			InventoryMenuWidget->SetFocus();
		}
		else
		{
			InventoryMenuWidget->SetVisibility(ESlateVisibility::Collapsed);
		}
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

