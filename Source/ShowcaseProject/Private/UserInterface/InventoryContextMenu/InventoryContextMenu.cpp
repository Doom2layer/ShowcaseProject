// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/InventoryContextMenu/InventoryContextMenu.h"
#include "Components/Button.h"
#include "Components/InventoryComponent/InventoryComponent.h"
#include "Components/WeaponSystemComponent/WeaponSystemComponent.h"
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
	if (AssignMeleeButton)
	{
		AssignMeleeButton->OnClicked.AddDynamic(this, &UInventoryContextMenu::OnAssignMeleeButtonClicked);
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

void UInventoryContextMenu::SetupButtonsForItemType() const
{
    if (!ItemReference)
    {
        UE_LOG(LogTemp, Warning, TEXT("ItemReference is null! Cannot setup buttons"));
        return;
    }

    // Hide all buttons by default
    EquipButton->SetVisibility(ESlateVisibility::Collapsed);
    AssignPrimaryButton->SetVisibility(ESlateVisibility::Collapsed);
    AssignSecondaryButton->SetVisibility(ESlateVisibility::Collapsed);
    AssignMeleeButton->SetVisibility(ESlateVisibility::Collapsed);
    UseButton->SetVisibility(ESlateVisibility::Collapsed);
    DiscardButton->SetVisibility(ESlateVisibility::Collapsed);
    ExamineButton->SetVisibility(ESlateVisibility::Collapsed);
    CombineButton->SetVisibility(ESlateVisibility::Collapsed);

    // Show buttons based on item type
    switch (ItemReference->ItemType)
    {
    case EItemType::Melee:
    case EItemType::Weapon:
        // Access WeaponCategory directly since it's a property of ItemBase
        switch (ItemReference->WeaponCategory)
        {
        case EWeaponCategory::Rifle:
        case EWeaponCategory::Shotgun:
            AssignPrimaryButton->SetVisibility(ESlateVisibility::Visible);
            EquipButton->SetVisibility(ESlateVisibility::Visible);
            break;
        case EWeaponCategory::Handgun:
            EquipButton->SetVisibility(ESlateVisibility::Visible);
            AssignSecondaryButton->SetVisibility(ESlateVisibility::Visible);
            break;
        case EWeaponCategory::Melee:
            EquipButton->SetVisibility(ESlateVisibility::Visible);
            AssignMeleeButton->SetVisibility(ESlateVisibility::Visible);
            break;
        }
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

    UE_LOG(LogTemp, Log, TEXT("UInventoryContextMenu::SetupButtonsForItemType: ItemType: %s, WeaponCategory: %s"),
        *UEnum::GetValueAsString(ItemReference->ItemType),
        *UEnum::GetValueAsString(ItemReference->WeaponCategory));

    // Show discard button if item is discardable
    DiscardButton->SetVisibility(ItemReference->ItemStatistics.bIsDiscardable ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UInventoryContextMenu::OnAssignMeleeButtonClicked()
{
	if (PlayerCharacter && ItemReference)
	{
		UWeaponSystemComponent* WeaponSystem = PlayerCharacter->GetWeaponSystem();
		if (WeaponSystem)
		{
			if (WeaponSystem->AssignWeaponToMeleeSlot(ItemReference))
			{
				UE_LOG(LogTemp, Log, TEXT("Successfully assigned item %s to melee slot"), *ItemReference->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Failed to assign item %s to melee slot"), *ItemReference->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("WeaponSystem is null! Cannot assign melee item"));
		}
		RemoveFromParent();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCharacter or ItemReference is null! Cannot assign melee item"));
	}
}

void UInventoryContextMenu::OnEquipButtonClicked()
{
	if (PlayerCharacter && ItemReference)
	{
		UWeaponSystemComponent* WeaponSystem = PlayerCharacter->GetWeaponSystem();
		if (WeaponSystem)
		{
			if (WeaponSystem->EquipWeapon(ItemReference))
			{
				UE_LOG(LogTemp, Log, TEXT("UInventoryContextMenu::OnEquipButtonClicked: Successfully equipped item %s."), *ItemReference->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryContextMenu::OnEquipButtonClicked: Failed to equip item %s."), *ItemReference->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryContextMenu::OnEquipButtonClicked: WeaponSystem is null! Cannot equip item."));
		}
		
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
		UWeaponSystemComponent* WeaponSystem = PlayerCharacter->GetWeaponSystem();
		if (WeaponSystem)
		{
            if (WeaponSystem->AssignWeaponToPrimarySlot(ItemReference))
			{
				UE_LOG(LogTemp, Log, TEXT("UInventoryContextMenu::OnAssignPrimaryButtonClicked: Successfully assigned item %s to primary slot."), *ItemReference->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryContextMenu::OnAssignPrimaryButtonClicked: Failed to assign item %s to primary slot."), *ItemReference->GetName());
			}
		}
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
		UWeaponSystemComponent* WeaponSystem = PlayerCharacter->GetWeaponSystem();
		if (WeaponSystem)
		{
            if (WeaponSystem->AssignWeaponToSecondarySlot(ItemReference))
			{
				UE_LOG(LogTemp, Log, TEXT("UInventoryContextMenu::OnAssignSecondaryButtonClicked: Successfully assigned item %s to secondary slot."), *ItemReference->GetName());
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("UInventoryContextMenu::OnAssignSecondaryButtonClicked: Failed to assign item %s to secondary slot."), *ItemReference->GetName());
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryContextMenu::OnAssignSecondaryButtonClicked: WeaponSystem is null! Cannot assign secondary item."));
		}
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




