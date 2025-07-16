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
	if (RequestedAddAmount <= 0 || FMath::IsNearlyZero(ItemIn->GetItemStackWeight()))
	{
		return 0; // No valid amount to add
	}

	int32 AmountToDistribute = RequestedAddAmount;
	// Check if the item already exists in the inventory and is not a full stack
	UItemBase* ExistingItemStack = FindNextPartialStack(ItemIn);
	// Distribure Item stack to existing items
	while (ExistingItemStack)
	{
		// calculate how many of the existing item would be needed to make a full stack
		const int32 AmountToMakeFullStack = CalculateNumberForFullStack(ExistingItemStack, AmountToDistribute);
		// calculate how many of the amounttomakefullstack can actually be carried based on weight capacity
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ExistingItemStack, AmountToMakeFullStack);
		// as long as the remaining amount to distribute is greater than 0, we can add to the existing stack
		if(WeightLimitAddAmount >0)
		{
			// adjust the existing item stack quantity and the inventory total weight
			ExistingItemStack->SetQuantity(ExistingItemStack->Quantity + WeightLimitAddAmount);
			InventoryTotalWeight += (ExistingItemStack->GetItemSingleWeight() * WeightLimitAddAmount);
			// adjust the count to be distributed
			AmountToDistribute -= WeightLimitAddAmount;

			ItemIn->SetQuantity(AmountToDistribute);
			//TODO: Refine this logic since going over weight capacity should not ever be possible
			
			//if max weight capacity is reached, we should stop adding to the existing stack
			if (InventoryTotalWeight >= InventoryWeightCapacity)
			{
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute; // Return the amount added
			}
		}
		else if (WeightLimitAddAmount <= 0)
		{
			if (AmountToDistribute != RequestedAddAmount)
			{
				//This block will be reached if distrubting the item stack to existing stacks has been successful, but the weight capacity is reached
				OnInventoryUpdated.Broadcast();
				return RequestedAddAmount - AmountToDistribute; // Return the amount added
			}
			return 0; // No valid amount to add
		}
		if (AmountToDistribute <= 0)
		{
			// all the requested amount has been added to existing stacks
			OnInventoryUpdated.Broadcast();
			return RequestedAddAmount; // All requested amount added
		}
		// check if there are more partial stacks of the same item
		ExistingItemStack = FindNextPartialStack(ItemIn);
	}

	//No more partial stacks found, we can add a new item stack
	if (InventoryContents.Num() + 1 <= InventorySlotsCapacity)
	{
		// attempt to add as many from the remaining item quantity that can fit the weight capacity
		const int32 WeightLimitAddAmount = CalculateWeightAddAmount(ItemIn, AmountToDistribute);
		if (WeightLimitAddAmount > 0)
		{
			// if there is still more item to distrbute and the weight limit allows it, we can add the item to the inventory
			if (WeightLimitAddAmount < AmountToDistribute)
			{
				// adjust the input item adn add a new stack with as many as can be held
				AmountToDistribute -= WeightLimitAddAmount;
				ItemIn->SetQuantity(WeightLimitAddAmount);
				// create a copy of the item since only a partial stack is being added
				AddNewItemToInventory(ItemIn->CreateItemCopy(), WeightLimitAddAmount);
				return RequestedAddAmount - AmountToDistribute; // Return the amount added
			}
			// otherwise, the full amount can be added
			AddNewItemToInventory(ItemIn, AmountToDistribute);
			return RequestedAddAmount; // All requested amount added
		}
	}
	OnInventoryUpdated.Broadcast();
	return RequestedAddAmount - AmountToDistribute; // Return the amount added
}

FItemAddResult UInventoryComponent::HandleAddItem(UItemBase* InputItem)
{
	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::HandleAddItem: Attempting to add item %s to inventory."), *InputItem->GetName());
	if (GetOwner())
	{
		const int32 InitialRequestedAddAmount = InputItem->Quantity;
		//Non stackable items
		if (!InputItem->ItemNumericData.bIsStackable)
		{
			UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::HandleAddItem: Item %s is non-stackable."), *InputItem->GetName());
			return HandleNonStackableItem(InputItem);
		}
		//Stackable
		const int32 StackableAmountAdded = HandleStackableItem(InputItem, InitialRequestedAddAmount);
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::HandleAddItem: Item %s is stackable, attempting to add %d."), *InputItem->GetName(), InitialRequestedAddAmount);
		if (StackableAmountAdded == InitialRequestedAddAmount)
		{
			UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::HandleAddItem: Added %d of item %s to inventory."), InitialRequestedAddAmount, *InputItem->GetName());
			return FItemAddResult::AddedAll(InitialRequestedAddAmount, FText::Format(
				FText::FromString("Added item {0} {1} to inventory."), InputItem->ItemTextData.Name, InitialRequestedAddAmount));			
		}
		if (StackableAmountAdded < InitialRequestedAddAmount && StackableAmountAdded > 0)
		{
			UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::HandleAddItem: Added %d of item %s to inventory, but not all requested amount."), StackableAmountAdded, *InputItem->GetName());
			return FItemAddResult::AddedPartial(StackableAmountAdded, FText::Format(
			FText::FromString("Partially added item {0} {1} to inventory."), InputItem->ItemTextData.Name, StackableAmountAdded
			));
		}
		if (StackableAmountAdded <= 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("UInventoryComponent::HandleAddItem: Could not add item %s to inventory, item would overflow weight or slots capacity."), *InputItem->GetName());
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
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AddNewItemToInventory: Adding existing item %s to inventory."), *ItemToAdd->GetName());
	}
	else
	{
		//used when splitting stacks or adding a new item from another source
		ItemToAdd = NewItem->CreateItemCopy();
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AddNewItemToInventory: Creating a copy of item %s to add to inventory."), *ItemToAdd->GetName());
	}
	ItemToAdd->OwningInventory = this;
	ItemToAdd->SetQuantity(AmountToAdd);
	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AddNewItemToInventory: Adding %d of item %s to inventory."), AmountToAdd, *ItemToAdd->GetName());

	InventoryContents.Add(ItemToAdd);
	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AddNewItemToInventory: Item %s added to inventory. Current inventory size: %d."), *ItemToAdd->GetName(), InventoryContents.Num());
	//Inventory Content
	UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AddNewItemToInventory: Current inventory contents:"));
	for (const UItemBase* Item : InventoryContents)
	{
		UE_LOG(LogTemp, Log, TEXT(" - %s (Quantity: %d)"), *Item->GetName(), Item->Quantity);
	}
	InventoryTotalWeight += ItemToAdd->GetItemStackWeight();
	//Print item Weapon category 
	if (ItemToAdd->ItemType == EItemType::Weapon)
	{
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AddNewItemToInventory: Added weapon item %s with category %s."), *ItemToAdd->GetName(), *UEnum::GetValueAsString(ItemToAdd->WeaponCategory));
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("UInventoryComponent::AddNewItemToInventory: Added non-weapon item %s."), *ItemToAdd->GetName());
	}
	OnInventoryUpdated.Broadcast();
}


