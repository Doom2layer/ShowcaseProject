// Fill out your copyright notice in the Description page of Project Settings.
#include "UserInterface/InventoryItemSlot/InventoryItemSlot.h"
#include "UserInterface/InventoryTooltip/InventoryTooltip.h"
#include  "UserInterface/InventoryContextMenu/InventoryContextMenu.h"
#include "Components/Border.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Items/ItemBase.h"
#include "Player/ShowcaseProjectCharacter.h"
#include "UserInterface/Inventory/InventoryPanel.h"

void UInventoryItemSlot::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if (InventoryTooltipClass)
	{
		UInventoryTooltip* Tooltip = CreateWidget<UInventoryTooltip>(this, InventoryTooltipClass);
		Tooltip->InventorySlotBeingHovered = this;
		SetToolTip(Tooltip);
	}
}

void UInventoryItemSlot::NativeConstruct()
{
	Super::NativeConstruct();
	UE_LOG(LogTemp, Log, TEXT("UInventoryItemSlot::NativeConstruct: Constructing inventory item slot for item %s."), *GetNameSafe(ItemReference));
	if (ItemReference)
	{
		switch (ItemReference->ItemQuality) {
		case EItemQuality::Shoddy:
			ItemBorder->SetBrushColor(FLinearColor::Gray);
			break;
		case EItemQuality::Common:
			ItemBorder->SetBrushColor(FLinearColor::White);
			break;
		case EItemQuality::Uncommon:
			ItemBorder->SetBrushColor(FLinearColor::Green);
			break;
		case EItemQuality::Masterwork:
			ItemBorder->SetBrushColor(FLinearColor::Blue);
			break;
		case EItemQuality::GrandMaster:
			ItemBorder->SetBrushColor(FLinearColor(0.5f, 0.0f, 0.5f, 1.0f));
			break;
		default:;
		}
		ItemImage->SetBrushFromTexture(ItemReference->ItemAssetData.Icon);

		if (ItemReference->ItemNumericData.bIsStackable)
		{
			ItemQuantity->SetText(FText::AsNumber(ItemReference->Quantity));
		}
		else
		{
			ItemQuantity->SetVisibility(ESlateVisibility::Collapsed);
		}
	}
}

FReply UInventoryItemSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton && ItemReference)
	{
		if (ContextMenuClass && OwningInventoryPanel)
		{
			UInventoryContextMenu* ContextMenu = CreateWidget<UInventoryContextMenu>(this, ContextMenuClass);
			ContextMenu->ItemReference = ItemReference;
			ContextMenu->PlayerCharacter = Cast<AShowcaseProjectCharacter>(GetOwningPlayerPawn());

			//Get the absolute position of the mouse click
			FVector2D MousePosition = InMouseEvent.GetScreenSpacePosition();

			//Add to viewport at the mouse position
			ContextMenu->AddToViewport(1000);

			//Position the context menu at the mouse position
			ContextMenu->SetPositionInViewport(MousePosition, false);

			OwningInventoryPanel->SetActiveContextMenu(ContextMenu);
		}
		
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UInventoryItemSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
}
