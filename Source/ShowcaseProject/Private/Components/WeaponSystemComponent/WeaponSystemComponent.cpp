// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponSystemComponent/WeaponSystemComponent.h"
#include "Items/ItemBase.h"
#include "Player/ShowcaseProjectCharacter.h"
#include "Weapons/WeaponBase.h"

// Sets default values for this component's properties
UWeaponSystemComponent::UWeaponSystemComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	PrimaryWeapon = nullptr;
	SecondaryWeapon = nullptr;
	MeleeWeapon = nullptr;
	CurrentEquippedWeapon = nullptr;
	OwningCharacter = nullptr;

	// ...
}


// Called when the game starts
void UWeaponSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

	OwningCharacter = Cast<AShowcaseProjectCharacter>(GetOwner());
	
}


// Called every frame
void UWeaponSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

bool UWeaponSystemComponent::EquipWeapon(UItemBase* WeaponToEquip)
{
	if (!WeaponToEquip || !OwningCharacter || WeaponToEquip->ItemType != EItemType::Weapon) return false;

	EWeaponSlot WeaponSlot = GetWeaponSlotFromCategory(WeaponToEquip->WeaponCategory);
	
	//Check if slot is available
	if (!IsHolsterAvailable(WeaponSlot))
	{
		UE_LOG(LogTemp, Warning, TEXT("Holster not available for weapon slot: %s"), *UEnum::GetValueAsString(WeaponSlot));
		return false;
	}

	//Unequip currently equipped weapon if any
	if (CurrentEquippedWeapon)
	{
		HolsterWeapon(GetWeaponSlotFromCategory(CurrentEquippedWeapon->GetWeaponItemData()->WeaponCategory));
	}
	//Spawn new weapon actor
	AWeaponBase* NewWeaponActor = SpawnWeaponActor(WeaponToEquip);
	
	if (!NewWeaponActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to spawn weapon actor for item: %s"), *WeaponToEquip->GetName());
		return false;
	}

	//Assign weapon to slot and equip it
	if (AssignWeaponToSlot(NewWeaponActor, WeaponSlot))
	{
		CurrentEquippedWeapon = NewWeaponActor;
		AttachWeaponToSocket(NewWeaponActor, FName("RightHandSocket"));
		UE_LOG(LogTemp, Log, TEXT("Equipped weapon: %s in slot: %s"), *WeaponToEquip->GetName(), *UEnum::GetValueAsString(WeaponSlot));
		return true;
	}
	return false;
}

bool UWeaponSystemComponent::AssignWeaponToPrimarySlot(UItemBase* WeaponToAssign)
{
	if (!WeaponToAssign || !OwningCharacter || WeaponToAssign->ItemType != EItemType::Weapon) return false;

	// Unequip any weapon currently in primary slot
	UnequipWeapon(EWeaponSlot::Primary);

	// Spawn new weapon actor
	AWeaponBase* NewWeaponActor = SpawnWeaponActor(WeaponToAssign);
	if (!NewWeaponActor) return false;

	// Assign to primary slot
	if (AssignWeaponToSlot(NewWeaponActor, EWeaponSlot::Primary))
	{
		// Set weapon state to holstered and attach to PrimaryHolster socket
		NewWeaponActor->SetWeaponState(EWeaponState::Holstered);
		AttachWeaponToSocket(NewWeaponActor, FName("PrimaryHolster"));
        
		UE_LOG(LogTemp, Log, TEXT("Successfully assigned weapon to primary slot and attached to PrimaryHolster"));
		return true;
	}

	// If assignment failed, destroy the weapon actor
	DestroyWeaponActor(NewWeaponActor);
	return false;
}

bool UWeaponSystemComponent::AssignWeaponToSecondarySlot(UItemBase* WeaponToAssign)
{
	if (!WeaponToAssign || !OwningCharacter || WeaponToAssign->ItemType != EItemType::Weapon) return false;

	// Unequip any weapon currently in secondary slot
	UnequipWeapon(EWeaponSlot::Secondary);

	// Spawn new weapon actor
	AWeaponBase* NewWeaponActor = SpawnWeaponActor(WeaponToAssign);
	if (!NewWeaponActor) return false;

	// Assign to secondary slot
	if (AssignWeaponToSlot(NewWeaponActor, EWeaponSlot::Secondary))
	{
		// Set weapon state to holstered and attach to SecondaryHolster socket
		NewWeaponActor->SetWeaponState(EWeaponState::Holstered);
		AttachWeaponToSocket(NewWeaponActor, FName("SecondaryHolster"));
        
		UE_LOG(LogTemp, Log, TEXT("Successfully assigned weapon to secondary slot and attached to SecondaryHolster"));
		return true;
	}

	// If assignment failed, destroy the weapon actor
	DestroyWeaponActor(NewWeaponActor);
	return false;
}

bool UWeaponSystemComponent::AssignWeaponToMeleeSlot(UItemBase* WeaponToAssign)
{
	if (!WeaponToAssign || !OwningCharacter || WeaponToAssign->ItemType != EItemType::Melee) return false;

	// Unequip any weapon currently in melee slot
	UnequipWeapon(EWeaponSlot::Melee);

	// Spawn new weapon actor
	AWeaponBase* NewWeaponActor = SpawnWeaponActor(WeaponToAssign);
	if (!NewWeaponActor) return false;

	// Assign to melee slot
	if (AssignWeaponToSlot(NewWeaponActor, EWeaponSlot::Melee))
	{
		// Set weapon state to holstered and attach to MeleeHolster socket
		NewWeaponActor->SetWeaponState(EWeaponState::Holstered);
		AttachWeaponToSocket(NewWeaponActor, FName("MeleeHolster"));
        
		UE_LOG(LogTemp, Log, TEXT("Successfully assigned weapon to melee slot and attached to MeleeHolster"));
		return true;
	}

	// If assignment failed, destroy the weapon actor
	DestroyWeaponActor(NewWeaponActor);
	return false;
}
bool UWeaponSystemComponent::UnequipWeapon(EWeaponSlot Slot)
{
	AWeaponBase* WeaponInSlot = GetWeaponInSlot(Slot);
	if (!WeaponInSlot || !OwningCharacter) return false;

	//If this is the currently equipped weapon, holster it
	if (CurrentEquippedWeapon == WeaponInSlot)
	{
		//Attach to holster
		if (CurrentEquippedWeapon->GetWeaponState() == EWeaponState::Equipped)
		{
			AttachWeaponToSocket(CurrentEquippedWeapon, CurrentEquippedWeapon->GetHolsterSocket());
		}
		CurrentEquippedWeapon = nullptr;
	}
	//Destroy weapon actor if it exists
	DestroyWeaponActor(WeaponInSlot);

	//Remove from slot
	UnassignWeaponFromSlot(Slot);

	UE_LOG(LogTemp, Log, TEXT("Unequipped weapon: %s from slot: %s"), *WeaponInSlot->GetName(), *UEnum::GetValueAsString(Slot));
	return true;
}

void UWeaponSystemComponent::HolsterWeapon(EWeaponSlot Slot)
{
	AWeaponBase* WeaponInSlot = GetWeaponInSlot(Slot);
	if (!WeaponInSlot || WeaponInSlot->GetWeaponState() != EWeaponState::Equipped) return;

	WeaponInSlot->SetWeaponState(EWeaponState::Holstered);

	AttachWeaponToSocket(WeaponInSlot, WeaponInSlot->GetHolsterSocket());

	if (CurrentEquippedWeapon == WeaponInSlot)
	{
		CurrentEquippedWeapon = nullptr;
	}

	UE_LOG(LogTemp, Log, TEXT("Holstered weapon: %s in slot: %s"), *WeaponInSlot->GetName(), *UEnum::GetValueAsString(Slot));
}

void UWeaponSystemComponent::DrawWeapon(EWeaponSlot Slot)
{
	AWeaponBase* WeaponInSlot = GetWeaponInSlot(Slot);
	if (!WeaponInSlot || WeaponInSlot->GetWeaponState() != EWeaponState::Holstered) return;

	// Holster current weapon first
	if (CurrentEquippedWeapon)
	{
		EWeaponSlot CurrentSlot = GetWeaponSlotFromCategory(CurrentEquippedWeapon->GetWeaponItemData()->WeaponCategory);
		HolsterWeapon(CurrentSlot);
	}

	// Draw the weapon
	WeaponInSlot->SetWeaponState(EWeaponState::Equipped);
	CurrentEquippedWeapon = WeaponInSlot;
	AttachWeaponToSocket(WeaponInSlot, FName("RightHandSocket"));
	UE_LOG(LogTemp, Log, TEXT("Drew weapon: %s in slot: %s"), *WeaponInSlot->GetName(), *UEnum::GetValueAsString(Slot));
}

void UWeaponSystemComponent::Fire()
{
	if (CurrentEquippedWeapon)
	{
		CurrentEquippedWeapon->Fire();
	}
}

void UWeaponSystemComponent::Reload()
{
	if (CurrentEquippedWeapon)
	{
		CurrentEquippedWeapon->Reload();
	}
}

void UWeaponSystemComponent::Attack()
{
	if (CurrentEquippedWeapon)
	{
		CurrentEquippedWeapon->Attack();
	}
}

bool UWeaponSystemComponent::IsHolsterAvailable(EWeaponSlot Slot) const
{
	switch (Slot) {
	case EWeaponSlot::Primary:
        return PrimaryWeapon == nullptr;
	case EWeaponSlot::Secondary:
		return SecondaryWeapon == nullptr;
	case EWeaponSlot::Melee:
		return MeleeWeapon == nullptr;
	default: return false; // Invalid slot
	}

}

AWeaponBase* UWeaponSystemComponent::GetEquippedWeapon() const
{
	return CurrentEquippedWeapon;
}

AWeaponBase* UWeaponSystemComponent::GetWeaponInSlot(EWeaponSlot Slot) const
{
	switch (Slot) {
	case EWeaponSlot::Primary:
		return PrimaryWeapon;
		break;
	case EWeaponSlot::Secondary:
		return SecondaryWeapon;
		break;
	case EWeaponSlot::Melee:
		return MeleeWeapon;
		break;
	default: 
		return nullptr; // Invalid slot
	}
}

void UWeaponSystemComponent::AttachWeaponToSocket(AWeaponBase* WeaponActor, const FName& SocketName)
{
	if (!WeaponActor || !OwningCharacter || SocketName == FName("None")) return;

	USkeletalMeshComponent* CharacterMesh = OwningCharacter->GetMesh();

	if (!CharacterMesh) return;

	WeaponActor->AttachToComponent(
		CharacterMesh,
		FAttachmentTransformRules::SnapToTargetIncludingScale,
		SocketName
	);
}

AWeaponBase* UWeaponSystemComponent::SpawnWeaponActor(UItemBase* WeaponItem)
{
	if (!WeaponItem || !OwningCharacter) return nullptr;

	UWorld* World = OwningCharacter->GetWorld();
	if (!World) return nullptr;
	
	// You'll need to determine weapon class from item data
	// For now using default - implement proper class mapping
	TSubclassOf<AWeaponBase> WeaponClass = AWeaponBase::StaticClass();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = OwningCharacter;
	SpawnParams.Instigator = OwningCharacter->GetInstigator();

	AWeaponBase* WeaponActor = World->SpawnActor<AWeaponBase>(WeaponClass, SpawnParams);
	if (WeaponActor)
	{
		WeaponActor->InitializeWeapon(WeaponItem);
		UE_LOG(LogTemp, Log, TEXT("Spawned weapon actor for: %s"), *WeaponItem->ItemTextData.Name.ToString());
	}
	return WeaponActor;
}

void UWeaponSystemComponent::DestroyWeaponActor(AWeaponBase* WeaponActor)
{
	if (WeaponActor)
	{
		UE_LOG(LogTemp, Log, TEXT("Destroyed weapon actor for: %s"), *WeaponActor->GetWeaponItemData()->ItemTextData.Name.ToString());
		WeaponActor->Destroy();		
	}
}

bool UWeaponSystemComponent::AssignWeaponToSlot(AWeaponBase* WeaponActor, EWeaponSlot Slot)
{
	if (!WeaponActor) return false;

	switch (Slot)
	{
	case EWeaponSlot::Primary:
		if (PrimaryWeapon) return false;
		PrimaryWeapon = WeaponActor;
		return true;
	case EWeaponSlot::Secondary:
		if (SecondaryWeapon) return false;
		SecondaryWeapon = WeaponActor;
		return true;
	case EWeaponSlot::Melee:
		if (MeleeWeapon) return false;
		MeleeWeapon = WeaponActor;
		return true;
	default:
		return false;
	}
}

void UWeaponSystemComponent::UnassignWeaponFromSlot(EWeaponSlot Slot)
{
	switch (Slot)
	{
	case EWeaponSlot::Primary:
		PrimaryWeapon = nullptr;
		break;
	case EWeaponSlot::Secondary:
		SecondaryWeapon = nullptr;
		break;
	case EWeaponSlot::Melee:
		MeleeWeapon = nullptr;
		break;
	default:
		UE_LOG(LogTemp, Warning, TEXT("UnassignWeaponFromSlot called with invalid slot: %s"), *UEnum::GetValueAsString(Slot));
		break;
		
	}
}

EWeaponSlot UWeaponSystemComponent::GetWeaponSlotFromCategory(EWeaponCategory Category)
{

	switch (Category) {
	case EWeaponCategory::Handgun:
		return EWeaponSlot::Secondary; // Handguns are typically secondary weapons
		break;
	case EWeaponCategory::Rifle:
	case EWeaponCategory::Shotgun:
		return EWeaponSlot::Primary; // Rifles and shotguns are typically primary weapons
		break;
	case EWeaponCategory::Melee:
		return EWeaponSlot::Melee; // Melee weapons have their own slot
		break;
		default: return EWeaponSlot::Primary; // Invalid category
	}


}

FName UWeaponSystemComponent::GetHolsterSocket(EWeaponSlot Slot)
{
	return FName("HolsterSocket"); // Default socket name, implement proper mapping later
}

