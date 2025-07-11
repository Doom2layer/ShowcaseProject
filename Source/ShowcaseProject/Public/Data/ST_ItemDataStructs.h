#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "ST_ItemDataStructs.generated.h"

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
struct FItemStatistics
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere)
	float Damage;
	
	UPROPERTY(EditAnywhere)
	float Range;
	
	UPROPERTY(EditAnywhere)
	float FireRate;

	UPROPERTY(EditAnywhere)
	float ReloadTime;

	UPROPERTY(EditAnywhere)
	float Accuracy;

	UPROPERTY(EditAnywhere)
	int32 MagazineSize;
    
	UPROPERTY(EditAnywhere)
	int32 MaxAmmo;
    
	UPROPERTY(EditAnywhere)
	float RecoilIntensity;
    
	UPROPERTY(EditAnywhere)
	float SpreadAngle;
    
	UPROPERTY(EditAnywhere)
	bool bIsAutomatic;
    
	UPROPERTY(EditAnywhere)
	float AimDownSightTime;

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

	UPROPERTY(EditAnywhere , Category="Item Data")
	EWeaponCategory WeaponCategory;
};
