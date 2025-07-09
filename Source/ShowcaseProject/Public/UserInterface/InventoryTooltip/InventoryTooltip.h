// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryTooltip.generated.h"

class UInventoryItemSlot;
class UTextBlock;

/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UInventoryTooltip : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
	UInventoryItemSlot* InventorySlotBeingHovered;
	
	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemName;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemType;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* DamageValue;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* AccuracyRating;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* UsageText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* ItemDescription;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* StackSizeText;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* StackSizeValue;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* StackWeightValue;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* MaxStackSize;
	
protected:
	virtual void NativeConstruct() override;
};
