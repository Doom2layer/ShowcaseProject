// Fill out your copyright notice in the Description page of Project Settings.

#include "UserInterface/Inventory/InventoryPanel.h"
#include "UserInterface/InventoryItemSlot/InventoryItemSlot.h"
#include "Components/TextBlock.h"
#include "Components/WrapBox.h"
#include "Components/InventoryComponent/InventoryComponent.h"
#include "Items/ItemBase.h"
#include "Player/ShowcaseProjectCharacter.h"
#include "UserInterface/InventoryContextMenu/InventoryContextMenu.h"

void UInventoryPanel::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	PlayerCharacter = Cast<AShowcaseProjectCharacter>(GetOwningPlayerPawn());
	
	if (PlayerCharacter)
	{
		InventoryReference = PlayerCharacter->GetInventory();
		if (InventoryReference)
		{
			InventoryReference->OnInventoryUpdated.AddUObject(this, &UInventoryPanel::RefreshInventory);
			SetInfoText();
		}
	}
}

void UInventoryPanel::SetInfoText() const
{
	//Setting the text for weight and capacity info
	WeightInfo->SetText(FText::Format(FText::FromString("{0}/{1}"), InventoryReference->GetInventoryTotalWeight(), InventoryReference->GetWeightCapacity()));
	CapacityInfo->SetText(FText::Format(FText::FromString("{0}/{1}"), InventoryReference->GetInventoryContents().Num(), InventoryReference->GetSlotsCapacity()));
}

void UInventoryPanel::RefreshInventory()
{
	UE_LOG(LogTemp, Log, TEXT("UInventoryPanel::RefreshInventory: InventoryReference is %s."), InventoryReference ? TEXT("valid") : TEXT("null"));
	UE_LOG(LogTemp, Log, TEXT("UInventoryPanel::RefreshInventory: InventoryPanel is %s."), InventoryPanel ? TEXT("valid") : TEXT("null"));
	UE_LOG(LogTemp, Log, TEXT("UInventoryPanel::RefreshInventory: InventoryItemSlotClass is %s."), InventoryItemSlotClass ? TEXT("valid") : TEXT("null"));
	
	UE_LOG(LogTemp, Log, TEXT("UInventoryPanel::RefreshInventory: Refreshing inventory panel for player %s."), *GetOwningPlayer()->GetName());
	if (InventoryReference && InventoryItemSlotClass)
	{
		UE_LOG(LogTemp, Log, TEXT("UInventoryPanel::RefreshInventory: Refreshing inventory panel."));
		InventoryPanel->ClearChildren();
		CloseActiveContextMenu();
		for (UItemBase* const& Item : InventoryReference->GetInventoryContents())
		{
			UE_LOG(LogTemp, Log, TEXT("UInventoryPanel::RefreshInventory: Adding item %s to inventory panel."), *Item->GetName());
			UInventoryItemSlot* ItemSlot = CreateWidget<UInventoryItemSlot>(this, InventoryItemSlotClass);
			ItemSlot->SetItemReference(Item);
			ItemSlot->SetOwningInventoryPanel(this);
			InventoryPanel->AddChildToWrapBox(ItemSlot);
		}
		SetInfoText();
	}
}

void UInventoryPanel::CloseActiveContextMenu()
{
	if (ActiveContextMenu)
	{
		ActiveContextMenu->RemoveFromParent();
		ActiveContextMenu = nullptr;
	}
}

void UInventoryPanel::SetActiveContextMenu(UInventoryContextMenu* NewContextMenu)
{
	CloseActiveContextMenu();
	ActiveContextMenu = NewContextMenu;
}

