#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Weapons/ProjectileBase.h"
#include "ST_ItemDataStructs.generated.h"

class AWeaponBase;

UENUM()
enum class EAmmoType : uint8
{
	PistolAmmo UMETA (DisplayName = "Pistol Ammo"),
	RifleAmmo UMETA (DisplayName = "Rifle Ammo"),
	ShotgunAmmo UMETA (DisplayName = "Shotgun Ammo"),
};

UENUM()
enum class EItemQuality : uint8
{
	Shoddy UMETA(DisplayName = "Shoddy"),
	Common UMETA(DisplayName = "Common"),
	Uncommon UMETA(DisplayName = "Uncommon"),
	Masterwork UMETA(DisplayName = "Masterwork"),
	GrandMaster UMETA(DisplayName = "Grand Master"),
};

UENUM()
enum class EItemType : uint8
{
		Weapon UMETA(DisplayName = "Weapon"),
		Melee UMETA(DisplayName = "Melee"),
		Ammo UMETA(DisplayName = "Ammo"),
		Health UMETA(DisplayName = "Health"),
		Key UMETA(DisplayName = "Key"),
		Document UMETA(DisplayName = "Document"),
};

UENUM()
enum class EWeaponCategory : uint8
{
	Handgun UMETA(DisplayName = "Handgun"),
	Rifle UMETA(DisplayName = "Rifle"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	Melee UMETA(DisplayName = "Melee")
};

USTRUCT()
struct FItemWeaponData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	TSubclassOf<AWeaponBase> WeaponActorClass;
	
	UPROPERTY(EditAnywhere, Category="Weapon Data")
	float Damage;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	EAmmoType AmmoType;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	int32 ShotgunPelletCount;
	
	UPROPERTY(EditAnywhere, Category="Weapon Data")
	float Range;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	float FireRate;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	float ReloadTime;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	float Accuracy;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	int32 MagazineSize;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	int32 MaxAmmo;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	float RecoilIntensity;
    
	UPROPERTY(EditAnywhere, Category="Weapon Data")
	float SpreadAngle;
    
	UPROPERTY(EditAnywhere, Category="Weapon Data")
	bool bIsAutomatic;
    
	UPROPERTY(EditAnywhere, Category="Weapon Data")
	float AimDownSightTime;
	
	UPROPERTY(EditAnywhere, Category="Weapon Data")
	UAnimMontage* ReloadMontage;

	// Projectile properties
	UPROPERTY(EditAnywhere, Category="Weapon Data")
	TSubclassOf<AProjectileBase> ProjectileClass;
	
	UPROPERTY(EditAnywhere, Category="Weapon Data")
	float ProjectileSpeed;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	float ProjectileGravityScale;

	// Effects
	UPROPERTY(EditAnywhere, Category="Weapon Data")
	UParticleSystem* MuzzleFlash = nullptr;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	USoundBase* FireSound = nullptr;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	USoundBase* EmptySound = nullptr;

	UPROPERTY(EditAnywhere, Category="Weapon Data")
	UAnimMontage* FireMontage = nullptr;
};

USTRUCT()
struct FItemAmmoData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category="Ammo Data")
	EAmmoType AmmoType;
    
	UPROPERTY(EditAnywhere, Category="Ammo Data")
	int32 AmmoPerBox;  // How many rounds per pickup/box
    
	UPROPERTY(EditAnywhere, Category="Ammo Data")
	float DamageModifier;  // Optional damage multiplier for special ammo types
    
	UPROPERTY(EditAnywhere, Category="Ammo Data")
	bool bIsPenetrating;  // Can penetrate armor/walls
    
	UPROPERTY(EditAnywhere, Category="Ammo Data")
	float PenetrationPower;
};

USTRUCT()
struct FItemStatistics
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float HealthRestoration;

	UPROPERTY(EditAnywhere)
	bool bIsDiscardable;
};

USTRUCT()
struct FItemTextData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere)
	FText Name;
	
	UPROPERTY(EditAnywhere)
	FText Description;
	
	UPROPERTY(EditAnywhere)
	FText InteractionText;
	
	UPROPERTY(EditAnywhere)
	FText UsageText;
};

USTRUCT()
struct FItemNumericData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float Weight;
	
	UPROPERTY(EditAnywhere)
	int32 MaxStackSize;

	UPROPERTY(EditAnywhere)
	bool bIsStackable;
};

USTRUCT()
struct FItemAssetData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	UTexture2D* Icon;

	UPROPERTY(EditAnywhere)
	UStaticMesh* Mesh;

	UPROPERTY(EditAnywhere)
	USkeletalMesh* SkeletalMesh;

	UPROPERTY(EditAnywhere)
	USoundBase* PickupSound;

	UPROPERTY(EditAnywhere)
	USoundBase* UseSound;
};


USTRUCT()
struct FItemData : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category="Item Data")
	FName ItemID;

	UPROPERTY(EditAnywhere , Category="Item Data")
	EWeaponCategory WeaponCategory;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemWeaponData WeaponData;

	UPROPERTY(EditAnywhere, Category="Item Data")
	FItemAmmoData AmmoData;

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

};
