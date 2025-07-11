// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/InventoryContextMenu/InventoryContextMenu.h"
#include "Components/Button.h"
#include "Components/InventoryComponent/InventoryComponent.h"
#include "Items/ItemBase.h"
#include "Player/ShowcaseProjectCharacter.h"

void UInventoryContextMenu::NativeConstruct()
{
	Super::NativeConstruct();

	if (EquipButton)
	{
		EquipButton->OnClicked.AddDynamic(this, &UInventoryContextMenu::OnUseButtonClicked);
	}
	if (UseButton)
	{
		UseButton->OnClicked.AddDynamic(this, &UInventoryContextMenu::OnUseButtonClicked);
	}
	if (AssignPrimaryButton)
	{
		AssignPrimaryButton->OnClicked.AddDynamic(this, &UInventoryContextMenu::OnAssignPrimaryButtonClicked);
	}
	if (AssignSecondaryButton)
	{
		AssignSecondaryButton->OnClicked.AddDynamic(this, &UInventoryContextMenu::OnAssignSecondaryButtonClicked);
	}
	if (ExamineButton)
	{
		ExamineButton->OnClicked.AddDynamic(this, &UInventoryContextMenu::OnExamineButtonClicked);
	}
	if (DiscardButton)
	{
		DiscardButton->OnClicked.AddDynamic(this, &UInventoryContextMenu::OnDiscardButtonClicked);
	}
	if (CombineButton)
	{
		CombineButton->OnClicked.AddDynamic(this, &UInventoryContextMenu::OnCombineButtonClicked);
	}

	SetupButtonsForItemType();
}

void UInventoryContextMenu::SetupButtonsForItemType()
{
	if (!ItemReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryContextMenu::OnUseButtonClicked: ItemReference is null! Cannot use item."));
		return;
	}

	//Hide all buttons by default
	EquipButton->SetVisibility(ESlateVisibility::Collapsed);
	AssignPrimaryButton->SetVisibility(ESlateVisibility::Collapsed);
	AssignSecondaryButton->SetVisibility(ESlateVisibility::Collapsed);
	UseButton->SetVisibility(ESlateVisibility::Collapsed);
	DiscardButton->SetVisibility(ESlateVisibility::Collapsed);
	ExamineButton->SetVisibility(ESlateVisibility::Collapsed);
	CombineButton->SetVisibility(ESlateVisibility::Collapsed);

	UE_LOG(LogTemp, Warning, TEXT("UInventoryContextMenu::SetupButtonsForItemType: Setting up buttons for item type: %s"), *UEnum::GetValueAsString(ItemReference->ItemType));
	//Show buttons based on item type
	switch (ItemReference->ItemType)
	{
	case EItemType::Weapon:
		ExamineButton->SetVisibility(ESlateVisibility::Visible);
		EquipButton->SetVisibility(ESlateVisibility::Visible);
		AssignPrimaryButton->SetVisibility(ESlateVisibility::Visible);
		AssignSecondaryButton->SetVisibility(ESlateVisibility::Visible);
		break;
	case EItemType::Melee:
		EquipButton->SetVisibility(ESlateVisibility::Visible);
		break;
	case EItemType::Ammo:
		break;
	case EItemType::Health:
		UseButton->SetVisibility(ESlateVisibility::Visible);
		ExamineButton->SetVisibility(ESlateVisibility::Visible);
		break;
	case EItemType::Key:
		UseButton->SetVisibility(ESlateVisibility::Visible);
		break;
	case EItemType::Document:
		ExamineButton->SetVisibility(ESlateVisibility::Visible);
		break;
	}
	//Show discard button if item is discardable
	DiscardButton->SetVisibility(ItemReference->ItemStatistics.bIsDiscardable ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);

}

void UInventoryContextMenu::OnEquipButtonClicked()
{
	if (PlayerCharacter && ItemReference)
	{
		PlayerCharacter->EquipWeapon(ItemReference);
		RemoveFromParent();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryContextMenu::OnEquipButtonClicked: PlayerCharacter or ItemReference is null! Cannot equip item."));
	}
}

void UInventoryContextMenu::OnUseButtonClicked()
{
	if (PlayerCharacter && ItemReference)
	{
		ItemReference->UseItem(PlayerCharacter);
		RemoveFromParent();
	}
}

void UInventoryContextMenu::OnAssignPrimaryButtonClicked()
{
	if (PlayerCharacter && ItemReference)
	{
		PlayerCharacter->AssignPrimary(ItemReference);
		RemoveFromParent();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryContextMenu::OnAssignPrimaryButtonClicked: PlayerCharacter or ItemReference is null! Cannot assign primary item."));
	}
}

void UInventoryContextMenu::OnAssignSecondaryButtonClicked()
{
	if (PlayerCharacter && ItemReference)
	{
		
		PlayerCharacter->AssignSecondary(ItemReference);
		RemoveFromParent();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UInventoryContextMenu::OnAssignSecondaryButtonClicked: PlayerCharacter or ItemReference is null! Cannot assign secondary item."));
	}
	
}

void UInventoryContextMenu::OnExamineButtonClicked()
{
	// Will add an inspection class that will render a scene capture 2d on the screen with the item in it
	RemoveFromParent();
}

void UInventoryContextMenu::OnDiscardButtonClicked()
{
	if (PlayerCharacter && ItemReference)
	{
		
		if (UInventoryComponent* InventoryReference = PlayerCharacter->GetInventory())
		{
			InventoryReference->RemoveSingleInstanceOfItem(ItemReference);
			RemoveFromParent();
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryContextMenu::OnDiscardButtonClicked: InventoryReference is null! Cannot discard item."));
		}
	}
}

void UInventoryContextMenu::OnCombineButtonClicked()
{
	// Will add a combine class that will allow the player to combine items in the inventory
	RemoveFromParent();
}




