// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryPanel.generated.h"

class UInventoryContextMenu;
class UInventoryComponent;
class UInventoryItemSlot;
class AShowcaseProjectCharacter;
class UWrapBox;
class UTextBlock;
/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UInventoryPanel : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void RefreshInventory();

	UFUNCTION()
	void CloseActiveContextMenu();

	void SetActiveContextMenu(UInventoryContextMenu* NewContextMenu);

	UPROPERTY()
	UInventoryContextMenu* ActiveContextMenu;

	UPROPERTY(meta=(BindWidget))
	UWrapBox* InventoryPanel;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* WeightInfo;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* CapacityInfo;

	UPROPERTY()
	AShowcaseProjectCharacter* PlayerCharacter;

	UPROPERTY()
	UInventoryComponent* InventoryReference;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemSlot> InventoryItemSlotClass;
	
protected:
	void SetInfoText() const;
	virtual void NativeOnInitialized() override;
};
