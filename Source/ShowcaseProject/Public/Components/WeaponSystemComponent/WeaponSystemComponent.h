// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ST_ItemDataStructs.h"
#include "WeaponSystemComponent.generated.h"

class AShowcaseProjectCharacter;
class AWeaponBase;
class UItemBase;
class UAnimMontage;

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	Primary UMETA(DisplayName = "Primary"),
	Secondary UMETA(DisplayName = "Secondary"),
	Melee UMETA(DisplayName = "Melee")
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOWCASEPROJECT_API UWeaponSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponSystemComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//Ammo Management
	UFUNCTION(BlueprintCallable, Category="Weapon System")
	FORCEINLINE	bool CanReloadFromInventory() const;

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	int32 GetAvailableAmmoInInventory(EAmmoType AmmoType) const;
	
	//Weapon Managment
	UFUNCTION(BlueprintCallable, Category="Weapon System")
	bool EquipWeapon(UItemBase* WeaponToEquip);

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	bool AssignWeaponToPrimarySlot(UItemBase* WeaponToAssign);

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	bool AssignWeaponToSecondarySlot(UItemBase* WeaponToAssign);

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	bool AssignWeaponToMeleeSlot(UItemBase* WeaponToAssign);
	
	UFUNCTION(BlueprintCallable, Category="Weapon System")
	bool UnequipWeapon(EWeaponSlot Slot);

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	void HolsterWeapon(EWeaponSlot Slot);

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	void DrawWeapon(EWeaponSlot Slot);

	//Weapon Actions
	UFUNCTION(BlueprintCallable, Category="Weapon System")
	void Fire();

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	void Reload();

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	void Attack();

	//Holster availability check
	UFUNCTION(BlueprintPure, Category="Weapon System")
	bool IsHolsterAvailable(EWeaponSlot Slot) const;

	UFUNCTION(BlueprintPure, Category="Weapon System")
	AWeaponBase* GetEquippedWeapon() const;

	UFUNCTION(BlueprintPure, Category="Weapon System")
	AWeaponBase* GetWeaponInSlot(EWeaponSlot Slot) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon System|Animation")
	UAnimMontage* EquipPrimaryMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon System|Animation")
	UAnimMontage* HolsterPrimaryMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon System|Animation")
	UAnimMontage* EquipSecondaryMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon System|Animation")
	UAnimMontage* HolsterSecondaryMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon System|Animation")
	UAnimMontage* EquipMeleeMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon System|Animation")
	UAnimMontage* HolsterMeleeMontage;

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	void DrawWeaponWithAnimation(EWeaponSlot Slot);

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	void HolsterWeaponWithAnimation(EWeaponSlot Slot);

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	void OnEquipAnimationComplete(EWeaponSlot Slot);

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	void OnHolsterAnimationComplete(EWeaponSlot Slot);

	UFUNCTION(BlueprintCallable, Category="Weapon System")
	EWeaponSlot GetSlotForWeapon(AWeaponBase* Weapon);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	//Weapon Slots
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon System")
	AWeaponBase* PrimaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon System")
	AWeaponBase* SecondaryWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon System")
	AWeaponBase* MeleeWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon System")
	AWeaponBase* CurrentEquippedWeapon;

	//Helper functions
	void AttachWeaponToSocket(AWeaponBase* WeaponActor, const FName& SocketName);

	AWeaponBase* SpawnWeaponActor(UItemBase* WeaponItem);

	void DestroyWeaponActor(AWeaponBase* WeaponActor);

	// Internal weapon management
	bool AssignWeaponToSlot(AWeaponBase* WeaponItem, EWeaponSlot Slot);

	void UnassignWeaponFromSlot(EWeaponSlot Slot);

	EWeaponSlot GetWeaponSlotFromCategory(EWeaponCategory Category);

private:	
	UPROPERTY()
	AShowcaseProjectCharacter* OwningCharacter;
	EWeaponSlot PendingWeaponSlot = EWeaponSlot::Primary;
	bool bIsWeaponAnimationPlaying = false;
	
	UAnimMontage* GetEquipMontageForSlot(EWeaponSlot Slot);
	UAnimMontage* GetHolsterMontageForSlot(EWeaponSlot Slot);
	void PlayWeaponMontage(UAnimMontage* Montage);
	bool bHasPendingWeaponSwitch = false;
};