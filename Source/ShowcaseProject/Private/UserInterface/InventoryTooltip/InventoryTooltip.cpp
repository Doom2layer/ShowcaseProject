// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/InventoryTooltip/InventoryTooltip.h"

#include "Components/TextBlock.h"
#include "Items/ItemBase.h"
#include "UserInterface/InventoryItemSlot/InventoryItemSlot.h"

void UInventoryTooltip::NativeConstruct()
{
	Super::NativeConstruct();

	const UItemBase* ItemBeingHovered = InventorySlotBeingHovered->GetItemReference();

	switch (ItemBeingHovered->ItemType) {
	case EItemType::Weapon:
		ItemType->SetText(FText::FromString("Weapon"));
		DamageValue->SetVisibility(ESlateVisibility::Visible);
		AccuracyRating->SetVisibility(ESlateVisibility::Visible);
		break;
	case EItemType::Melee:
		ItemType->SetText(FText::FromString("Melee"));
		DamageValue->SetVisibility(ESlateVisibility::Visible);
		AccuracyRating->SetVisibility(ESlateVisibility::Visible);
		break;
	case EItemType::Ammo:
		ItemType->SetText(FText::FromString("Ammo"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		AccuracyRating->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EItemType::Health:
		ItemType->SetText(FText::FromString("Consumable"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		AccuracyRating->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EItemType::Key:
		ItemType->SetText(FText::FromString("Key Items"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		AccuracyRating->SetVisibility(ESlateVisibility::Collapsed);
		break;
	case EItemType::Document:
		ItemType->SetText(FText::FromString("Document"));
		DamageValue->SetVisibility(ESlateVisibility::Collapsed);
		AccuracyRating->SetVisibility(ESlateVisibility::Collapsed);
		break;
	default:;
	}

	ItemName->SetText(ItemBeingHovered->ItemTextData.Name);
	DamageValue->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.Damage));
	AccuracyRating->SetText(FText::AsNumber(ItemBeingHovered->ItemStatistics.Accuracy));
	UsageText->SetText(ItemBeingHovered->ItemTextData.UsageText);
	ItemDescription->SetText(ItemBeingHovered->ItemTextData.Description);
	StackWeightValue->SetText(FText::AsNumber(ItemBeingHovered->GetItemStackWeight()));

	if (ItemBeingHovered->ItemNumericData.bIsStackable)
	{
		MaxStackSize->SetText(FText::AsNumber(ItemBeingHovered->ItemNumericData.MaxStackSize));
	}
	else
	{
		MaxStackSize->SetVisibility(ESlateVisibility::Collapsed);
	}
	
}
