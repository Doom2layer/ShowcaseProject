// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemSlot.generated.h"

class UInventoryContextMenu;
class UInventoryTooltip;
class UItemBase;
class UBorder;
class UImage;
class UTextBlock;
class UInventoryPanel;
/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UInventoryItemSlot : public UUserWidget
{
	GENERATED_BODY()

public:
	FORCEINLINE void SetItemReference(UItemBase* ItemIn) { ItemReference = ItemIn; }
	FORCEINLINE UItemBase* GetItemReference() const { return ItemReference; }
	FORCEINLINE void SetOwningInventoryPanel(UInventoryPanel* InventoryPanel) { OwningInventoryPanel = InventoryPanel; }
protected:

	UPROPERTY()
	UInventoryPanel* OwningInventoryPanel;

	UPROPERTY(EditDefaultsOnly, Category="Inventory Slot")
	TSubclassOf<UInventoryContextMenu> ContextMenuClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Inventory Slot")
	TSubclassOf<UInventoryTooltip> InventoryTooltipClass;

	UPROPERTY(VisibleAnywhere, Category="Inventory Slot")
	UItemBase* ItemReference;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="Inventory Slot")
	UBorder* ItemBorder;
	
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="Inventory Slot")
	UImage* ItemImage;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category="Inventory Slot")
	UTextBlock* ItemQuantity;
	
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
};
