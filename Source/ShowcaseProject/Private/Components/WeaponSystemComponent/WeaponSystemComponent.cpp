// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/WeaponSystemComponent/WeaponSystemComponent.h"
#include "Items/ItemBase.h"
#include "Components/InventoryComponent/InventoryComponent.h"
#include "Player/ShowcaseProjectCharacter.h"
#include "UserInterface/ShowcaseHUD/ShowcaseHUD.h"
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
	EquipPrimaryMontage = nullptr;
	HolsterPrimaryMontage = nullptr;
	EquipSecondaryMontage = nullptr;
	HolsterSecondaryMontage = nullptr;
	EquipMeleeMontage = nullptr;
	HolsterMeleeMontage = nullptr;

	// ...
}

void UWeaponSystemComponent::DrawWeaponWithAnimation(EWeaponSlot Slot)
{
	if (bIsWeaponAnimationPlaying) return; // Prevent re-entry

	AWeaponBase* WeaponInSlot = GetWeaponInSlot(Slot);
	if (!WeaponInSlot || WeaponInSlot->GetWeaponState() != EWeaponState::Holstered) return;

	// If another weapon is currently equipped, we need to holster it first
	if (CurrentEquippedWeapon && CurrentEquippedWeapon != WeaponInSlot)
	{
		EWeaponSlot CurrentSlot = GetSlotForWeapon(CurrentEquippedWeapon);
		PendingWeaponSlot = Slot; // Store which weapon we want to equip next
		bHasPendingWeaponSwitch = true;
        
		HolsterWeaponWithAnimation(CurrentSlot);
		return;
	}

	UAnimMontage* EquipMontage = GetEquipMontageForSlot(Slot);
	if (EquipMontage)
	{
		bIsWeaponAnimationPlaying = true;
		PlayWeaponMontage(EquipMontage);
		UE_LOG(LogTemp, Log, TEXT("Starting equip animation for slot: %s"), *UEnum::GetValueAsString(Slot));
	}
	else
	{
		// Fallback to instant equip if no montage is assigned
		DrawWeapon(Slot);
	}
}

void UWeaponSystemComponent::HolsterWeaponWithAnimation(EWeaponSlot Slot)
{
	if (bIsWeaponAnimationPlaying) return; // Prevent re-entry
	
	AWeaponBase* WeaponInSlot = GetWeaponInSlot(Slot);
	if (!WeaponInSlot || WeaponInSlot->GetWeaponState() != EWeaponState::Equipped) return;

	UAnimMontage* HolsterMontage = GetHolsterMontageForSlot(Slot);
	if (HolsterMontage)
	{
		bIsWeaponAnimationPlaying = true;
		PlayWeaponMontage(HolsterMontage);
		UE_LOG(LogTemp, Log, TEXT("Starting holster animation for slot: %s"), *UEnum::GetValueAsString(Slot));
	}
	else
	{
		// Fallback to instant holster if no montage is assigned
		HolsterWeapon(Slot);
	}
}

void UWeaponSystemComponent::OnEquipAnimationComplete(EWeaponSlot Slot)
{
	UE_LOG(LogTemp, Log, TEXT("Equip animation complete for slot: %s"), *UEnum::GetValueAsString(Slot));
	bIsWeaponAnimationPlaying = false;
	DrawWeapon(Slot);
}

void UWeaponSystemComponent::OnHolsterAnimationComplete(EWeaponSlot Slot)
{
	UE_LOG(LogTemp, Log, TEXT("Holster animation complete for slot: %s"), *UEnum::GetValueAsString(Slot));
	bIsWeaponAnimationPlaying = false;
	HolsterWeapon(Slot);
    
	// Check if we have a pending weapon switch
	if (bHasPendingWeaponSwitch)
	{
		bHasPendingWeaponSwitch = false;
        
		// Now equip the pending weapon
		UAnimMontage* EquipMontage = GetEquipMontageForSlot(PendingWeaponSlot);
		if (EquipMontage)
		{
			bIsWeaponAnimationPlaying = true;
			PlayWeaponMontage(EquipMontage);
			UE_LOG(LogTemp, Log, TEXT("Starting pending equip animation for slot: %s"), *UEnum::GetValueAsString(PendingWeaponSlot));
		}
		else
		{
			// Fallback to instant equip if no montage is assigned
			DrawWeapon(PendingWeaponSlot);
		}
	}
}

EWeaponSlot UWeaponSystemComponent::GetSlotForWeapon(AWeaponBase* Weapon)
{
	if (!Weapon) return EWeaponSlot::Primary;
    
	if (Weapon == PrimaryWeapon) return EWeaponSlot::Primary;
	if (Weapon == SecondaryWeapon) return EWeaponSlot::Secondary;
	if (Weapon == MeleeWeapon) return EWeaponSlot::Melee;
    
	return EWeaponSlot::Primary; // fallback

}

UAnimMontage* UWeaponSystemComponent::GetEquipMontageForSlot(EWeaponSlot Slot)
{
	switch (Slot)
	{
	case EWeaponSlot::Primary:
		return EquipPrimaryMontage;
	case EWeaponSlot::Secondary:
		return EquipSecondaryMontage;
	case EWeaponSlot::Melee:
		return EquipMeleeMontage;
	default:
		return nullptr;
	}
}

UAnimMontage* UWeaponSystemComponent::GetHolsterMontageForSlot(EWeaponSlot Slot)
{
	switch (Slot)
	{
	case EWeaponSlot::Primary:
		return HolsterPrimaryMontage;
	case EWeaponSlot::Secondary:
		return HolsterSecondaryMontage;
	case EWeaponSlot::Melee:
		return HolsterMeleeMontage;
	default:
		return nullptr;
	}
}

void UWeaponSystemComponent::PlayWeaponMontage(UAnimMontage* Montage)
{
	
	if (!Montage || !OwningCharacter) return;

	UAnimInstance* AnimInstance = OwningCharacter->GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		AnimInstance->Montage_Play(Montage);
		UE_LOG(LogTemp, Log, TEXT("Playing weapon montage: %s"), *Montage->GetName());
	}
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

bool UWeaponSystemComponent::CanReloadFromInventory() const
{
	if (!CurrentEquippedWeapon || !OwningCharacter)
	{
		return false;
	}

	// Check if weapon needs reload
	if (!CurrentEquippedWeapon->CanReload())
	{
		return false;
	}

	// Check if we have ammo in inventory
	EAmmoType RequiredAmmoType = CurrentEquippedWeapon->GetRequiredAmmoType();
	return GetAvailableAmmoInInventory(RequiredAmmoType) > 0;
}

int32 UWeaponSystemComponent::GetAvailableAmmoInInventory(EAmmoType AmmoType) const
{
	if (!OwningCharacter) return 0;
    
	UInventoryComponent* Inventory = OwningCharacter->GetInventory();
	if (!Inventory) return 0;
    
	int32 TotalAmmo = 0;
	for (UItemBase* Item : Inventory->GetInventoryContents())
	{
		if (Item && Item->ItemType == EItemType::Ammo && 
			Item->AmmoData.AmmoType == AmmoType)
		{
			TotalAmmo += Item->Quantity;
		}
	}
	return TotalAmmo;
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
	if (OwningCharacter)
	{
		AShowcaseHUD* HUD = Cast<AShowcaseHUD>(OwningCharacter->GetWorld()->GetFirstPlayerController()->GetHUD());
		if (HUD)
		{
			HUD->UpdateWeaponDisplay(nullptr); // No weapon equipped
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Holstered weapon: %s in slot: %s"), *WeaponInSlot->GetName(), *UEnum::GetValueAsString(Slot));
}

void UWeaponSystemComponent::DrawWeapon(EWeaponSlot Slot)
{
    AWeaponBase* WeaponInSlot = GetWeaponInSlot(Slot);
    if (!WeaponInSlot || WeaponInSlot->GetWeaponState() != EWeaponState::Holstered) return;

    // Draw the weapon
    WeaponInSlot->SetWeaponState(EWeaponState::Equipped);
    CurrentEquippedWeapon = WeaponInSlot;
    
    // Attach to right hand socket
    AttachWeaponToSocket(WeaponInSlot, FName("RightHandSocket"));
    
    // Check if skeletal mesh component exists AND has a valid skeletal mesh assigned
    if (WeaponInSlot->GetSkeletalMeshComponent() != nullptr && 
        WeaponInSlot->GetSkeletalMeshComponent()->GetSkeletalMeshAsset() != nullptr)
    {
        // Check if the Handle socket exists on the skeletal mesh
        if (WeaponInSlot->GetSkeletalMeshComponent()->DoesSocketExist(FName("Handle")))
        {
            CurrentEquippedWeapon->GetSkeletalMeshComponent()->SetRelativeLocation(
                CurrentEquippedWeapon->GetSkeletalMeshComponent()->GetSocketTransform(
                    FName("Handle"),
                    ERelativeTransformSpace::RTS_Component
                ).GetLocation() *= -1.0f
            );

            CurrentEquippedWeapon->GetSkeletalMeshComponent()->SetRelativeRotation(
                CurrentEquippedWeapon->GetSkeletalMeshComponent()->GetSocketTransform(
                    FName("Handle"),
                    ERelativeTransformSpace::RTS_Component
                ).GetRotation().Rotator()
            );
            
            UE_LOG(LogTemp, Log, TEXT("Drew weapon using skeletal mesh: %s"), *WeaponInSlot->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Skeletal mesh exists but no Handle socket found for weapon: %s"), *WeaponInSlot->GetName());
        }
    }
    // Check if static mesh component exists AND has a valid static mesh assigned
    else if (WeaponInSlot->GetStaticMeshComponent() != nullptr && 
             WeaponInSlot->GetStaticMeshComponent()->GetStaticMesh() != nullptr)
    {
        // Check if the Handle socket exists on the static mesh
        if (WeaponInSlot->GetStaticMeshComponent()->DoesSocketExist(FName("Handle")))
        {
            CurrentEquippedWeapon->GetStaticMeshComponent()->SetRelativeLocation(
                CurrentEquippedWeapon->GetStaticMeshComponent()->GetSocketTransform(
                    FName("Handle"),
                    ERelativeTransformSpace::RTS_Component
                ).GetLocation() *= -1.0f
            );

            CurrentEquippedWeapon->GetStaticMeshComponent()->SetRelativeRotation(
                CurrentEquippedWeapon->GetStaticMeshComponent()->GetSocketTransform(
                    FName("Handle"),
                    ERelativeTransformSpace::RTS_Component
                ).GetRotation().Rotator()
            );
            
            UE_LOG(LogTemp, Log, TEXT("Drew weapon using static mesh: %s"), *WeaponInSlot->GetName());
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Static mesh exists but no Handle socket found for weapon: %s"), *WeaponInSlot->GetName());
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("No valid mesh found for weapon: %s"), *WeaponInSlot->GetName());
    }
	if (OwningCharacter)
	{
		AShowcaseHUD* HUD = Cast<AShowcaseHUD>(OwningCharacter->GetWorld()->GetFirstPlayerController()->GetHUD());
		if (HUD)
		{
			HUD->UpdateWeaponDisplay(CurrentEquippedWeapon);
		}
	}
    UE_LOG(LogTemp, Log, TEXT("Drew weapon: %s in slot: %s"), *WeaponInSlot->GetName(), *UEnum::GetValueAsString(Slot));
}

void UWeaponSystemComponent::StartFire()
{
	if (CurrentEquippedWeapon)
	{
		CurrentEquippedWeapon->StartFire();
	}
}

void UWeaponSystemComponent::StopFire()
{
	if (CurrentEquippedWeapon)
	{
		CurrentEquippedWeapon->StopFire();
	}
}

void UWeaponSystemComponent::Reload()
{
	if (CurrentEquippedWeapon)
	{
		UE_LOG(LogTemp, Log, TEXT("Reloading weapon: %s"), *CurrentEquippedWeapon->GetName());
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
	case EWeaponSlot::Secondary:
		return SecondaryWeapon;
	case EWeaponSlot::Melee:
		return MeleeWeapon;
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
    TSubclassOf<AWeaponBase> WeaponClass = WeaponItem->WeaponData.WeaponActorClass;
	if (!WeaponClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("No weapon class specified for item: %s, using default"), *WeaponItem->GetName());
		WeaponClass = AWeaponBase::StaticClass(); // Fallback to default
	}
	
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