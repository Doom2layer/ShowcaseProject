// Fill out your copyright notice in the Description page of Project Settings.


#include "World/Pickup.h"

#include "Components/InventoryComponent/InventoryComponent.h"
#include "Items/ItemBase.h"
#include "Player/ShowcaseProjectCharacter.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	PickupMesh = CreateDefaultSubobject<UStaticMeshComponent>("Pickup Mesh");
	PickupMesh->SetSimulatePhysics((false));
	SetRootComponent(PickupMesh);
	
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();
	
	InitializePickup(UItemBase::StaticClass(), ItemQuantity);
}

void APickup::InitializePickup(const TSubclassOf<UItemBase> BaseClass, const int32 InQuantity)
{
	if (ItemDataTable && !DesiredItemID.IsNone())
	{
		const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString());

		ItemReference = NewObject<UItemBase>(this, BaseClass);

		ItemReference->ItemID = ItemData->ItemID;
		ItemReference->WeaponCategory = ItemData->WeaponCategory;
		ItemReference->ItemType = ItemData->ItemType;
		ItemReference->ItemQuality = ItemData->ItemQuality;
		ItemReference->ItemNumericData = ItemData->ItemNumericData;
		ItemReference->ItemTextData = ItemData->ItemTextData;
		ItemReference->ItemAssetData = ItemData->ItemAssetData;
		ItemReference->WeaponData = ItemData->WeaponData;
		ItemReference->AmmoData = ItemData->AmmoData;
		ItemReference->ItemStatistics = ItemData->ItemStatistics;
		

		InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);

		PickupMesh->SetStaticMesh(ItemData->ItemAssetData.Mesh);

		UpdateInteractableData();
	}
}

void APickup::InitializeDrop(UItemBase* ItemToDrop, const int32 InQuantity)
{
	ItemReference = ItemToDrop;
	InQuantity <= 0 ? ItemReference->SetQuantity(1) : ItemReference->SetQuantity(InQuantity);
	ItemReference->ItemNumericData.Weight = ItemToDrop->GetItemSingleWeight();
	PickupMesh->SetStaticMesh(ItemReference->ItemAssetData.Mesh);
	
	UpdateInteractableData();
}

void APickup::UpdateInteractableData()
{
	InstanceInteractableData.InteractableType = EInteractableType::Pickup;
	InstanceInteractableData.Action = ItemReference->ItemTextData.InteractionText;
	InstanceInteractableData.Name = ItemReference->ItemTextData.Name;
	InstanceInteractableData.Quantity = ItemReference->Quantity;
	InteractableData = InstanceInteractableData;
}

void APickup::BeginFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(true);
	}
}

void APickup::EndFocus()
{
	if (PickupMesh)
	{
		PickupMesh->SetRenderCustomDepth(false);
	}
}

void APickup::Interact(AShowcaseProjectCharacter* PlayerCharacter)
{
	if (PlayerCharacter)
	{
		TakePickup(PlayerCharacter);
	}
}

void APickup::TakePickup(const AShowcaseProjectCharacter* Taker)
{
	if (!IsPendingKillPending())
	{
		if (ItemReference)
		{
			if (UInventoryComponent* PlayerInventory = Taker->GetInventory())
			{
				const FItemAddResult AddResult = PlayerInventory->HandleAddItem(ItemReference);

				switch (AddResult.OperationResult)
				{
				case EItemAddResult::IAR_NoItemAdded:
					UE_LOG(LogTemplateCharacter, Error, TEXT("Pickup %s taken by %s. No item added."), *GetNameSafe(this), *GetNameSafe(Taker));
					break;
				case EItemAddResult::IAR_PartialAmountItemAdded:
					UE_LOG(LogTemplateCharacter, Warning, TEXT("Pickup %s taken by %s. Partial amount added."), *GetNameSafe(this), *GetNameSafe(Taker));
					UpdateInteractableData();
					Taker->UpdateInteractionWidget();
					break;
				case EItemAddResult::IAR_AllItemAdded:
					UE_LOG(LogTemplateCharacter, Log, TEXT("Pickup %s taken by %s. All items added."), *GetNameSafe(this), *GetNameSafe(Taker));
					Destroy();
					break;
				}
				UE_LOG(LogTemplateCharacter, Log, TEXT("Pickup %s taken by %s. Result: %s"), *GetNameSafe(this), *GetNameSafe(Taker), *AddResult.ResultMessage.ToString());
			}
			else
			{
				UE_LOG(LogTemplateCharacter, Error, TEXT("Pickup %s taken by %s. No inventory component found."), *GetNameSafe(this), *GetNameSafe(Taker));
			}
		}
		else
		{
			UE_LOG(LogTemplateCharacter, Error, TEXT("Pickup %s taken by %s. No item reference found."), *GetNameSafe(this), *GetNameSafe(Taker));
		}
	}
}

void APickup::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName ChangedPropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (ChangedPropertyName == GET_MEMBER_NAME_CHECKED(APickup, DesiredItemID))
	{
		if (ItemDataTable)
		{
			if (const FItemData* ItemData = ItemDataTable->FindRow<FItemData>(DesiredItemID, DesiredItemID.ToString()))
			{
				PickupMesh->SetStaticMesh(ItemData->ItemAssetData.Mesh);
			}
		}
	}
}

