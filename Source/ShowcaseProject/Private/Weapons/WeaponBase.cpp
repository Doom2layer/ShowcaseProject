// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponBase.h"

#include "Items/ItemBase.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create mesh component
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SkeletalMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComponent"));

	// Set root Component (We'll switch between these based on what mesh we're using)
	RootComponent = StaticMeshComponent;

	//Initially hide skeletal mesh
	SkeletalMeshComponent->SetVisibility(false);
	StaticMeshComponent->SetVisibility(true);

	bUsingSkeletalMesh = false;
	WeaponState = EWeaponState::Unequipped;
	bCanFire = true;

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	// Initialize weapon components
	SetupMeshComponents();
	
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AWeaponBase::InitializeWeapon(UItemBase* WeaponItem)
{
	WeaponItemData = WeaponItem;
	WeaponState = EWeaponState::Unequipped;
	bCanFire = true;
	// Initialize weapon stats from item data
	if (WeaponItemData)
	{
		// Set the weapon mesh from item data - prioritize skeletal mesh
		SetWeaponMesh(WeaponItemData->ItemAssetData.Mesh, WeaponItemData->ItemAssetData.SkeletalMesh);
        
		UE_LOG(LogTemp, Log, TEXT("Initialized weapon: %s"), *WeaponItemData->ItemTextData.Name.ToString());
	}
}

void AWeaponBase::Fire()
{
	if (CanFire())
	{
		UE_LOG(LogTemp, Log, TEXT("Firing weapon: %s"), *WeaponItemData->ItemTextData.Name.ToString());
		// Implement firing logic here
		bCanFire = false; // Set cooldown or similar logic
		GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &AWeaponBase::ResetFireCooldown, WeaponItemData->ItemStatistics.FireRate, false);
	}
}

void AWeaponBase::Attack()
{
	if (WeaponState == EWeaponState::Equipped && WeaponItemData)
	{
		UE_LOG(LogTemp, Log, TEXT("Melee attack with weapon: %s"), *WeaponItemData->ItemTextData.Name.ToString());
		// Implement melee attack logic here
	}
}

bool AWeaponBase::CanFire() const
{
	return bCanFire && WeaponState == EWeaponState::Equipped;
}

bool AWeaponBase::CanReload() const
{
    return WeaponState == EWeaponState::Equipped && WeaponItemData != nullptr;
}

void AWeaponBase::Reload()
{
	if (CanReload())
	{
		UE_LOG(LogTemp, Log, TEXT("Reloading weapon: %s"), *WeaponItemData->ItemTextData.Name.ToString());
		// Implement reload logic here
	}
}

void AWeaponBase::SetWeaponState(EWeaponState NewState)
{
	WeaponState = NewState;
	UE_LOG(LogTemp, Log, TEXT("Weapon state changed to: %s"), *UEnum::GetValueAsString(NewState));
}

FName AWeaponBase::GetHolsterSocket() const
{
	switch (WeaponItemData->WeaponCategory)
	{
	case EWeaponCategory::Rifle:
	case EWeaponCategory::Shotgun:
		return FName("PrimaryHolster");
	case EWeaponCategory::Handgun:
		return FName("SecondaryHolster");
	case EWeaponCategory::Melee:
		return FName("MeleeHolster");
	default:
		return FName("None");
	}
}

void AWeaponBase::SetWeaponMesh(UStaticMesh* StaticMesh, USkeletalMesh* SkeletalMesh)
{
	// Prioritize skeletal mesh if available
	if (SkeletalMesh)
	{
		SkeletalMeshComponent->SetSkeletalMesh(SkeletalMesh);
		SkeletalMeshComponent->SetVisibility(true);
		StaticMeshComponent->SetVisibility(false);

		//switch root component to skeletal mesh
		RootComponent = SkeletalMeshComponent;
		bUsingSkeletalMesh = true;

		UE_LOG(LogTemp, Log, TEXT("Set weapon to use SkeletalMesh: %s"), *SkeletalMesh->GetName());
	}
	else if (StaticMesh)
	{
		StaticMeshComponent->SetStaticMesh(StaticMesh);
		StaticMeshComponent->SetVisibility(true);
		SkeletalMeshComponent->SetVisibility(false);
        
		// Switch root component to static mesh
		RootComponent = StaticMeshComponent;
		bUsingSkeletalMesh = false;
        
		UE_LOG(LogTemp, Log, TEXT("Set static mesh for weapon"));

	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SetWeaponMesh called with null StaticMesh and SkeletalMesh! No mesh set for weapon."));
	}
}

UPrimitiveComponent* AWeaponBase::GetActiveMeshComponent() const
{
	return bUsingSkeletalMesh ? static_cast<UPrimitiveComponent*>(SkeletalMeshComponent) : static_cast<UPrimitiveComponent*>(StaticMeshComponent);
}


void AWeaponBase::SetupMeshComponents()
{
	// Setup collision and other properties for both mesh types
	if (StaticMeshComponent)
	{
		StaticMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		StaticMeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	}

	if (SkeletalMeshComponent)
	{
		SkeletalMeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SkeletalMeshComponent->SetCollisionResponseToAllChannels(ECR_Block);
	}
}

void AWeaponBase::ResetFireCooldown()
{
	bCanFire = true;
}

