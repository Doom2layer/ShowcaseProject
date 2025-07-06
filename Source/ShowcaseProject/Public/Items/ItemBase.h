// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/ST_ItemDataStructs.h"
#include "ItemBase.generated.h"

/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UItemBase : public UObject
{
	GENERATED_BODY()

public:
	// PROPERTIES

	// UPROPERTY()
	// UInventoryComponent* OwningInventory;
	
	UPROPERTY(VisibleAnywhere, Category="Item Data", meta=(UIMin = "1", UIMax = "100"))
	int32 Quantity;
	
	UPROPERTY(EditAnywhere, Category="Item Data")
	FName ItemID;	

	UPROPERTY(EditAnywhere, Category="Item Data")
	EItemType ItemType;

	UPROPERTY(EditAnywhere, Category="Item Data")
	EItemQuality ItemQuality;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemStatistics ItemStatistics;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemTextData ItemTextData;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemNumericData ItemNumericData;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemAssetData ItemAssetData;

	// FUNCTIONS
	UItemBase();
	
	UFUNCTION(Category= "Item Data")
	UItemBase* CreateItemCopy() const;
	
	UFUNCTION(Category= "Item Data")
	FORCEINLINE float GetItemStackWeight() const {return Quantity * ItemNumericData.Weight;};

	UFUNCTION(Category= "Item Data")
	FORCEINLINE float GetItemSingleWeight() const {return ItemNumericData.Weight;};

	UFUNCTION(Category= "Item Data")
	FORCEINLINE bool IsFullItemStack() const {return Quantity == ItemNumericData.MaxStackSize;};

	UFUNCTION(Category= "Item Data")
	void SetQuantity(const int32 NewQuantity);
	
	UFUNCTION(Category= "Item Data")
	virtual void UseItem(AShowcaseProjectCharacter *Character);
	
protected:
	bool operator==(const FName& OtherID) const{ return ItemID == OtherID;  };
};
