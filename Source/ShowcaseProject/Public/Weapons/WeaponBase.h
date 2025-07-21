// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"	
#include "GameFramework/Actor.h"
#include "Items/ItemBase.h"
#include "WeaponBase.generated.h"

class AShowcaseProjectCharacter;

UENUM()
enum class EWeaponState: uint8
{
	Unequipped UMETA(DisplayName = "Unequipped"),
	Equipped UMETA(DisplayName = "Equipped"),
	Holstered UMETA(DisplayName = "Holstered")
};

class UItemBase;

UCLASS()
class SHOWCASEPROJECT_API AWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeaponBase();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	FORCEINLINE int32 GetCurrentAmmoInMagazine() const { return CurrentAmmoInMagazine; }

	UFUNCTION(BlueprintPure, Category = "Weapon|Ammo")
	FORCEINLINE int32 GetCurrentReserveAmmo() const { return CurrentReserveAmmo; }

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	int32 GetMaxMagazineSize() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	FORCEINLINE bool IsReloading() const { return bIsReloading; }
	
	UFUNCTION(BlueprintCallable, Category = "Weapon|Ammo")
	FORCEINLINE bool NeedsReload() const { return CurrentAmmoInMagazine < GetMaxMagazineSize() && CurrentReserveAmmo > 0; }	
	
	// Initialize weapon with item data
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void InitializeWeapon(UItemBase* WeaponItem);
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFire();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StopFire();
	
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Attack();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanFire() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool CanReload() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void Reload();

	// Weapon state management
	UFUNCTION(BlueprintCallable)
	void SetWeaponState(EWeaponState NewState);

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UItemBase* GetWeaponItemData() const { return WeaponItemData; }

	UFUNCTION(BlueprintCallable)
	FORCEINLINE UStaticMeshComponent *GetStaticMeshComponent() const { return StaticMeshComponent; }	

	UFUNCTION(BlueprintCallable)
	FORCEINLINE USkeletalMeshComponent *GetSkeletalMeshComponent() const { return SkeletalMeshComponent; }	
	
	UFUNCTION(BlueprintCallable)
	FName GetHolsterSocket() const;

	// Mesh management functions
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void SetWeaponMesh(UStaticMesh* StaticMesh, USkeletalMesh* SkeletalMesh = nullptr);

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	UPrimitiveComponent* GetActiveMeshComponent() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool IsUsingSkeletalMesh() const { return bUsingSkeletalMesh; }

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void PlayGunEffects();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void StartFireCooldown();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	void FireBullet();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EAmmoType GetRequiredAmmoType() const;

	FTimerHandle ReloadTimerHandle;

	FTimerHandle FireRateTimerHandle;
	
	FTimerHandle AutoFireTimerHandle;
	
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 CurrentAmmoInMagazine;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|Ammo")
	int32 CurrentReserveAmmo;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon|State")
	bool bIsReloading;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* SkeletalMeshComponent;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UItemBase* WeaponItemData;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	EWeaponState WeaponState;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bCanFire;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bUsingSkeletalMesh;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	bool HasAmmoInInventory() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	int32 GetAmmoFromInventory(int32 AmountNeeded);

	UPROPERTY()
	AShowcaseProjectCharacter* OwningCharacter;

	void SetupMeshComponents();
	

	void ResetFireCooldown();

};
