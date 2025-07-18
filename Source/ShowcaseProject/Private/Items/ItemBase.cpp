// Fill out your copyright notice in the Description page of Project Settings.


#include "Items/ItemBase.h"
#include "Weapons/ProjectileBase.h"
#include "Components/InventoryComponent/InventoryComponent.h"
#include "Player/ShowcaseProjectCharacter.h"

UItemBase::UItemBase() : bIsCopy(false), bIsPickup(false)
{
}

void UItemBase::ResetItemFlags()
{
	bIsCopy = false;
	bIsPickup = false;
}

UItemBase* UItemBase::CreateItemCopy() const
{
	UItemBase* ItemCopy = NewObject<UItemBase>(StaticClass());

	ItemCopy->ItemID = this->ItemID;
	ItemCopy->WeaponCategory = this->WeaponCategory;
	ItemCopy->Quantity = this->Quantity;
	ItemCopy->ItemType = this->ItemType;
	ItemCopy->ItemQuality = this->ItemQuality;
	ItemCopy->ItemStatistics = this->ItemStatistics;
	ItemCopy->ItemTextData = this->ItemTextData;
	ItemCopy->ItemNumericData = this->ItemNumericData;
	ItemCopy->ItemAssetData = this->ItemAssetData;
	ItemCopy->WeaponData = this->WeaponData;
	ItemCopy->AmmoData = this->AmmoData;
	ItemCopy->bIsCopy = true;

	return ItemCopy;
}

void UItemBase::SetQuantity(const int32 NewQuantity)
{
	UE_LOG(LogTemp, Log, TEXT("Setting quantity of item %s to %d"), *GetNameSafe(this), NewQuantity);
	if (NewQuantity != Quantity)
	{
		Quantity = FMath::Clamp(NewQuantity, 0, ItemNumericData.bIsStackable ? ItemNumericData.MaxStackSize : 1);

		if(OwningInventory)
		{
			if(Quantity<0)
			{
				OwningInventory->RemoveSingleInstanceOfItem(this);
			}
		}
	}
}

void UItemBase::UseItem(AShowcaseProjectCharacter* Character)
{
	UE_LOG(LogTemp, Log, TEXT("Using item: %s (ID: %s) - Quantity: %d"),
	   *ItemTextData.Name.ToString(),
	   *ItemID.ToString(),
	   Quantity);

	// Base implementation - child classes should override this
	UE_LOG(LogTemp, Warning, TEXT("UseItem called on base ItemBase class. This should be overridden in child classes."));

}
