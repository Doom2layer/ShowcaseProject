// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/InventoryComponent/InventoryComponent.h"

#include "Items/ItemBase.h"

// Sets default values for this component's properties
UInventoryComponent::UInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

// Called when the game starts
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

UItemBase* UInventoryComponent::FindMatchingItem(UItemBase* ItemToFind) const
{
	if (ItemToFind)
	{
		if (InventoryContents.Contains(ItemToFind))
		{
			return ItemToFind;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextItemByID(UItemBase* ItemToFind) const
{
	if (ItemToFind)
	{
		if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByKey(ItemToFind))
		{
			return *Result;
		}
	}
	return nullptr;
}

UItemBase* UInventoryComponent::FindNextPartialStack(UItemBase* ItemToFind) const
{
	if (const TArray<TObjectPtr<UItemBase>>::ElementType* Result = InventoryContents.FindByPredicate([&ItemToFind](const UItemBase* Item)
			{
				return Item->ItemID == ItemToFind->ItemID && !Item->IsFullItemStack();
			})
		)
	{
		return *Result;
	}
	return nullptr;
}

int32 UInventoryComponent::CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAddAmount) const
{
	const int32 WeightMaxAddAmount = FMath::FloorToInt((GetWeightCapacity() - InventoryTotalWeight) / ItemIn->GetItemSingleWeight());
	if (WeightMaxAddAmount >= RequestedAddAmount)
	{
		return RequestedAddAmount;
	}
	return WeightMaxAddAmount;
}

int32 UInventoryComponent::CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount) const
{
	const int32 AddAmountForFullStack = StackableItem->ItemNumericData.MaxStackSize - StackableItem->Quantity;

	return FMath::Min(InitialRequestedAddAmount, AddAmountForFullStack);
}

void UInventoryComponent::RemoveSingleInstanceOfItem(UItemBase* ItemToRemove)
{
	InventoryContents.RemoveSingle(ItemToRemove);
	OnInventoryUpdated.Broadcast();
}

int32 UInventoryComponent::RemoveAmountOfItem(UItemBase* ItemToRemove, int32 AmountToRemove)
{
	const int32 ActualAmountToRemove = FMath::Min(AmountToRemove, ItemToRemove->Quantity);
	ItemToRemove->SetQuantity(ItemToRemove->Quantity - ActualAmountToRemove);
	InventoryTotalWeight -= AmountToRemove * ItemToRemove->GetItemSingleWeight();
	OnInventoryUpdated.Broadcast();
	return ActualAmountToRemove;
}

void UInventoryComponent::SplitExistingStack(UItemBase* ItemToSplit, const int32 AmountToSplit)
{
	if (!(InventoryContents.Num() + 1 > InventorySlotsCapacity))
	{
		RemoveAmountOfItem(ItemToSplit, AmountToSplit);
		AddNewItemToInventory(ItemToSplit, AmountToSplit);
	}
}

FItemAddResult UInventoryComponent::HandleNonStackableItem(UItemBase* ItemIn)
{
	//Check if the input item has a valid weight
	if (FMath::IsNearlyZero(ItemIn->GetItemSingleWeight()) || ItemIn->GetItemSingleWeight() < 0)
	{
		return FItemAddResult::AddedNone(FText::Format(FText::FromString("Could not add item {0} to inventory, item has invalid weight."), ItemIn->ItemTextData.Name));
	}
	// will the item weight overflow the inventory weight capacity?
	if (InventoryTotalWeight + ItemIn->GetItemSingleWeight() > GetWeightCapacity())
	{
		return FItemAddResult::AddedNone(FText::Format(FText::FromString("Could not add item {0} to inventory, item would overflow weight capacity."), ItemIn->ItemTextData.Name));
	}
	// will the item overflow the inventory slots capacity?
	if (InventoryContents.Num() + 1 > InventorySlotsCapacity)
	{
		return FItemAddResult::AddedNone(FText::Format(FText::FromString("Could not add item {0} to inventory, item would overflow slots capacity."), ItemIn->ItemTextData.Name));
	}

	AddNewItemToInventory(ItemIn, 1);
	return FItemAddResult::AddedAll(1,FText::Format(FText::FromString("Added item {0} {1} to inventory."), ItemIn->ItemTextData.Name, 1));
}

int32 UInventoryComponent::HandleStackableItem(UItemBase* ItemIn, int32 RequestedAddAmount)
{
	return 1; // Placeholder for stackable item handling logic
}

FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* InputItem)
{
	if (GetOwner())
	{
		const int32 InitialRequestedAddAmount = InputItem->Quantity;
		//Non stackable items
		if (!InputItem->ItemNumericData.bIsStackable)
		{
			return HandleNonStackableItem(InputItem);
		}
		//Stackable
		const int32 StackableAmountAdded = HandleStackableItem(InputItem, InitialRequestedAddAmount);

		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(
				FText::FromString("Added item {0} {1} to inventory."), InputItem->ItemTextData.Name, InitialRequestedAddAmount));			
		}
		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			return FItemAddResult::AddedPartial(StackableAmountAdded, FText::Format(
			FText::FromString("Partially added item {0} {1} to inventory."), InputItem->ItemTextData.Name, StackableAmountAdded
			));
		}
		if (StackableAmountAdded <= 0)
		{
			return FItemAddResult::AddedNone(FText::Format(
			FText::FromString("Could not add item {0} to inventory, item would overflow weight or slots capacity."), InputItem->ItemTextData.Name
			));
		}
	}
	// check(false);
	UE_LOG(LogTemp, Error, TEXT("UInventoryComponent::HandleAddItem: Owner is null or invalid! Cannot add item to inventory."));
	return FItemAddResult::AddedNone(FText::FromString("An error occurred while trying to add item to inventory."));
}

void UInventoryComponent::AddNewItemToInventory(UItemBase* NewItem, int32 AmountToAdd)
{
	UItemBase* ItemToAdd;

	if (NewItem->bIsCopy || NewItem->bIsPickup)
	{
		// if the item is already a copy or a pickup, we create a new instance of it
		ItemToAdd = NewItem;
		ItemToAdd->ResetItemFlags();
	}
	else
	{
		//used when splitting stacks or adding a new item from another source
		ItemToAdd = NewItem->CreateItemCopy();
	}
	ItemToAdd->OwningInventory = this;
	ItemToAdd->SetQuantity(AmountToAdd);

	InventoryContents.Add(ItemToAdd);
	InventoryTotalWeight += ItemToAdd->GetItemStackWeight();
	OnInventoryUpdated.Broadcast();
}


