// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnInventoryUpdated);

class UItemBase;

UENUM(BlueprintType)
enum class EItemAddResult : uint8
{
	IAR_NoItemAdded UMETA(DisplayName = "No Item Added"),
	IAR_PartialAmountItemAdded UMETA(DisplayName = "Partial Amount Item Added"),
	IAR_AllItemAdded UMETA(DisplayName = "All Items Added"),
};

USTRUCT(BlueprintType)
struct FItemAddResult
{
	GENERATED_USTRUCT_BODY()

	FItemAddResult() : ActualAmountAdded(0), OperationResult(EItemAddResult::IAR_NoItemAdded), ResultMessage(FText::GetEmpty()) {};

	//Actual number of items added to the inventory
	UPROPERTY(BlueprintReadOnly, Category="Item Add Result")
	int32 ActualAmountAdded;

	// Enum indicating the result of the item addition operation
	UPROPERTY(BlueprintReadOnly, Category="Item Add Result")
	EItemAddResult OperationResult;

	//Message providing additional context about the result of the item addition operation
	UPROPERTY(BlueprintReadOnly, Category="Item Add Result")
	FText ResultMessage;

	static FItemAddResult AddedNone(const FText& ErrorMessage)
	{
		FItemAddResult AddedNoneResult;
		AddedNoneResult.ActualAmountAdded = 0;
		AddedNoneResult.OperationResult = EItemAddResult::IAR_NoItemAdded;
		AddedNoneResult.ResultMessage = ErrorMessage;
		return AddedNoneResult;
	}
	static FItemAddResult AddedPartial(const int32 PartialAmountAdded, const FText& ErrorMessage)
	{
		FItemAddResult AddedPartialResult;
		AddedPartialResult.ActualAmountAdded = PartialAmountAdded;
		AddedPartialResult.OperationResult = EItemAddResult::IAR_PartialAmountItemAdded;
		AddedPartialResult.ResultMessage = ErrorMessage;
		return AddedPartialResult;
		
	}
	static FItemAddResult AddedAll(const int32 AmountAdded, const FText& Message)
	{
		FItemAddResult AddedAllResult;
		AddedAllResult.ActualAmountAdded = AmountAdded;
		AddedAllResult.OperationResult = EItemAddResult::IAR_AllItemAdded;
		AddedAllResult.ResultMessage = Message;
		return AddedAllResult;
	}
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOWCASEPROJECT_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	//Properties
	FOnInventoryUpdated OnInventoryUpdated;
	
	
	//Functions
	// Sets default values for this component's properties
	UInventoryComponent();
	UFUNCTION(Category="Inventory")
	UItemBase* FindMatchingItem(UItemBase* ItemToFind) const;

	UFUNCTION(Category="Inventory")
	UItemBase* FindNextItemByID(UItemBase* ItemToFind) const;

	UFUNCTION(Category="Inventory")
	UItemBase* FindNextPartialStack(UItemBase* ItemToFind) const;
	
	UFUNCTION(Category="Inventory")
	FItemAddResult HandleAddItem(UItemBase* InputItem);

	UFUNCTION(Category="Inventory")
	void RemoveSingleInstanceOfItem(UItemBase* ItemToRemove);

	UFUNCTION(Category="Inventory")
	int32 RemoveAmountOfItem(UItemBase* ItemToRemove, int32 AmountToRemove);

	UFUNCTION(Category="Inventory")
	void SplitExistingStack(UItemBase* ItemToSplit, const int32 AmountToSplit);

	// Getters
	UFUNCTION(Category="Inventory")
	FORCEINLINE float GetInventoryTotalWeight() const { return InventoryTotalWeight; };

	UFUNCTION(Category="Inventory")
	FORCEINLINE float GetWeightCapacity() const { return  InventoryWeightCapacity; };

	UFUNCTION(Category="Inventory")
	FORCEINLINE int32 GetSlotsCapacity() const { return InventorySlotsCapacity; };

	UFUNCTION(Category="Inventory")
	FORCEINLINE TArray<UItemBase*> GetInventoryContents() const { return InventoryContents; };

	//Setters
	UFUNCTION(Category="Inventory")
	FORCEINLINE void SetSlotsCapacity(const int32 NewSlotsCapacity) { InventorySlotsCapacity = NewSlotsCapacity; };

	UFUNCTION(Category="Inventory")
	FORCEINLINE void SetWeightCapacity(const float NewWeightCapacity) { InventoryWeightCapacity = NewWeightCapacity; };
	
protected:
	//Properties
	UPROPERTY(VisibleAnywhere, Category="Inventory")
	float InventoryTotalWeight;
	UPROPERTY(EditInstanceOnly, Category="Inventory")
	int32 InventorySlotsCapacity;
	UPROPERTY(EditInstanceOnly, Category="Inventory")
	float InventoryWeightCapacity;
	UPROPERTY(VisibleAnywhere, Category="Inventory")
	TArray<TObjectPtr<UItemBase>> InventoryContents;

	
	
	//Functions
	// Called when the game starts
	virtual void BeginPlay() override;
	FItemAddResult HandleNonStackableItem(UItemBase* ItemIn, int32 RequestedAddAmount);
	int32 HandleStackableItem(UItemBase* ItemIn, int32 RequestedAddAmount);
	int32 CalculateWeightAddAmount(UItemBase* ItemIn, int32 RequestedAddAmount) const;
	int32 CalculateNumberForFullStack(UItemBase* StackableItem, int32 InitialRequestedAddAmount) const;

	void AddNewItemToInventory(UItemBase* NewItem, int32 AmountToAdd);
};


