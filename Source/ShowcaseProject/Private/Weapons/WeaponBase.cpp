// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapons/WeaponBase.h"
#include "Player/ShowcaseProjectCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InventoryComponent/InventoryComponent.h"

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
	bCanFire = true; // Initialize to true so weapon can fire initially
	bIsReloading = false;

}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	// Initialize weapon components
	SetupMeshComponents();
	bCanFire = true; 
}

// Called every frame
void AWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

int32 AWeaponBase::GetMaxMagazineSize() const
{
	if (WeaponItemData && WeaponItemData->ItemType == EItemType::Weapon)
	{
		return WeaponItemData->WeaponData.MagazineSize;
	}
    
	UE_LOG(LogTemp, Warning, TEXT("GetMaxMagazineSize: WeaponItemData is null or not a weapon type"));
	return 0;
}

void AWeaponBase::InitializeWeapon(UItemBase* WeaponItem)
{
	if (!WeaponItem) return;
    
	WeaponItemData = WeaponItem;
	OwningCharacter = Cast<AShowcaseProjectCharacter>(GetOwner());

	// Set ammo values from weapon data, with fallbacks
	int32 MagSize = WeaponItemData->WeaponData.MagazineSize;
	int32 MaxAmmo = WeaponItemData->WeaponData.MaxAmmo;
    
	// Fallback values if data table has 0s
	if (MagSize <= 0)
	{
		switch (WeaponItemData->WeaponCategory)
		{
		case EWeaponCategory::Handgun:
			MagSize = 15;
			break;
		case EWeaponCategory::Rifle:
			MagSize = 30;
			break;
		case EWeaponCategory::Shotgun:
			MagSize = 8;
			break;
		default:
			MagSize = 10;
			break;
		}
		WeaponItemData->WeaponData.MagazineSize = MagSize;
	}
    
	if (MaxAmmo <= 0)
	{
		MaxAmmo = MagSize * 3; // 3 full magazines worth
		WeaponItemData->WeaponData.MaxAmmo = MaxAmmo;

	}
    
	CurrentAmmoInMagazine = MagSize;
	CurrentReserveAmmo = MaxAmmo;

	// Set weapon state
	WeaponState = EWeaponState::Unequipped;
	bIsReloading = false;
	bCanFire = true;
    
	// Setup mesh based on item data
	SetWeaponMesh(WeaponItem->ItemAssetData.Mesh, WeaponItem->ItemAssetData.SkeletalMesh);
    
	UE_LOG(LogTemp, Log, TEXT("Initialized weapon: %s with %d/%d ammo"), 
		   *WeaponItem->ItemTextData.Name.ToString(),
		   CurrentAmmoInMagazine,
		   CurrentReserveAmmo);

	// Debug: Print all weapon data values
	UE_LOG(LogTemp, Warning, TEXT("=== Weapon Data Debug ==="));
	UE_LOG(LogTemp, Warning, TEXT("Weapon Name: %s"), *WeaponItemData->ItemTextData.Name.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Magazine Size: %d"), WeaponItemData->WeaponData.MagazineSize);
	UE_LOG(LogTemp, Warning, TEXT("Max Ammo: %d"), WeaponItemData->WeaponData.MaxAmmo);
	UE_LOG(LogTemp, Warning, TEXT("Damage: %f"), WeaponItemData->WeaponData.Damage);
	UE_LOG(LogTemp, Warning, TEXT("Fire Rate: %f"), WeaponItemData->WeaponData.FireRate);
	UE_LOG(LogTemp, Warning, TEXT("Can fire %s"), bCanFire ? TEXT("true") : TEXT("false"));
	UE_LOG(LogTemp, Warning, TEXT("========================"));
}

void AWeaponBase::StartFire()
{
	UE_LOG(LogTemp, Log, TEXT("Weapon %s starting fire"), *GetNameSafe(this));
	UE_LOG(LogTemp, Log, TEXT("Checking Ammo: %d in magazine, %d in reserve"),
		   CurrentAmmoInMagazine,
		   CurrentReserveAmmo);
	// Check if we have ammo
	if (CurrentAmmoInMagazine <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot fire - no ammo in magazine"));
		//Play empty click sound
		if (WeaponItemData->WeaponData.EmptySound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				this,
				WeaponItemData->WeaponData.EmptySound,
				GetActorLocation()
			);
		}
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Weapon %s can fire"), *GetNameSafe(this));
	// Set fire flag
	bCanFire = false;

	//Play gun effects
	if (WeaponItemData->WeaponData.bIsAutomatic)
	{
		UE_LOG(LogTemp, Log, TEXT("Weapon %s is automatic, setting up fire timer"), *GetNameSafe(this));
		//For autmatic weapons set up a timer to fire repeatedly
		GetWorldTimerManager().SetTimer(
			FireRateTimerHandle,
			this,
			&AWeaponBase::FireBullet,
			WeaponItemData->WeaponData.FireRate,
			true,
			0.0f
		);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("Weapon %s is single shot, firing immediately"), *GetNameSafe(this));
		FireBullet();
		// For single shot weapons, reset fire cooldown immediately
		StartFireCooldown();
	}
}

void AWeaponBase::StopFire()
{
	// Clear the firing timer for automatic weapons
	if (GetWorldTimerManager().IsTimerActive(FireRateTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(FireRateTimerHandle);
	}

	// For automatic weapons, allow firing again immediately when trigger is released
	if (WeaponItemData && WeaponItemData->WeaponData.bIsAutomatic)
	{
		bCanFire = true;
	}
	// For single-shot weapons, bCanFire is controlled by the cooldown timer
}

void AWeaponBase::PlayGunEffects()
{
	if (!WeaponItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot play gun effects - WeaponItemData is null"));
		return;
	}

	//Play muzzle flash effect
	if (WeaponItemData->WeaponData.MuzzleFlash)
	{
		UPrimitiveComponent* MeshComp = GetActiveMeshComponent();
		if (MeshComp)
		{
			//Try to find the muzzle socket
			FName MuzzleSocket = FName("Muzzle");
			FVector MuzzleLocation;
			FRotator MuzzleRotation;

			if (bUsingSkeletalMesh && SkeletalMeshComponent->DoesSocketExist(MuzzleSocket))
			{
				FTransform SocketTransform = SkeletalMeshComponent->GetSocketTransform(MuzzleSocket);
				MuzzleLocation = SocketTransform.GetLocation();
				MuzzleRotation = SocketTransform.GetRotation().Rotator();
			}
			else if (!bUsingSkeletalMesh && StaticMeshComponent->DoesSocketExist(MuzzleSocket))
			{
				FTransform SocketTransform = StaticMeshComponent->GetSocketTransform(MuzzleSocket);
				MuzzleLocation = SocketTransform.GetLocation();
				MuzzleRotation = SocketTransform.GetRotation().Rotator();
			}
			else
			{
				//Fall back to component location + forward offset
				MuzzleLocation = MeshComp->GetComponentLocation() + MeshComp->GetForwardVector() * 100.0f; // Adjust offset as needed
				MuzzleRotation = MeshComp->GetComponentRotation();
			}
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				WeaponItemData->WeaponData.MuzzleFlash,
				MuzzleLocation,
				MuzzleRotation
			);
		}
	}
	// Play fire sound
	if (WeaponItemData->WeaponData.FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			WeaponItemData->WeaponData.FireSound,
			GetActorLocation()
		);
	}
	//Play Fire animation montage if available
	if (WeaponItemData->WeaponData.FireMontage)
	{
		//Get the character that owns this weapon
		if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
		{
			if (USkeletalMeshComponent* OwnerMesh = OwningPawn->FindComponentByClass<USkeletalMeshComponent>())
			{
				if (UAnimInstance* AnimInstance = OwnerMesh->GetAnimInstance())
				{
					AnimInstance->Montage_Play(WeaponItemData->WeaponData.FireMontage, 1.0f);
				}
			}
		}
	}
}

void AWeaponBase::FireBullet()
{
    // if (!CanFire()) return;

    // Consume ammo
    CurrentAmmoInMagazine--;

    // Get spawn location and rotation from weapon
    FVector SpawnLocation;
    FRotator SpawnRotation;
    
    if (IsUsingSkeletalMesh() && SkeletalMeshComponent->DoesSocketExist(FName("Muzzle")))
    {
        FTransform MuzzleTransform = SkeletalMeshComponent->GetSocketTransform(FName("Muzzle"));
        SpawnLocation = MuzzleTransform.GetLocation();
        SpawnRotation = MuzzleTransform.Rotator();
    }
    else if (StaticMeshComponent->DoesSocketExist(FName("Muzzle")))
    {
        FTransform MuzzleTransform = StaticMeshComponent->GetSocketTransform(FName("Muzzle"));
        SpawnLocation = MuzzleTransform.GetLocation();
        SpawnRotation = MuzzleTransform.Rotator();
    }
    else
    {
        SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
        SpawnRotation = GetActorRotation();
    }

    // Check if this is a shotgun (uses pellets)
	if (WeaponItemData->WeaponCategory == EWeaponCategory::Shotgun && WeaponItemData->WeaponData.ShotgunPelletCount > 1)
	{
		for (int32 i = 0; i < WeaponItemData->WeaponData.ShotgunPelletCount; i++)
		{
			// Calculate spread for each pellet
			float RandomPitch = FMath::FRandRange(-WeaponItemData->WeaponData.SpreadAngle, WeaponItemData->WeaponData.SpreadAngle);
			float RandomYaw = FMath::FRandRange(-WeaponItemData->WeaponData.SpreadAngle, WeaponItemData->WeaponData.SpreadAngle);
            
			// Create pellet rotation with spread
			FRotator PelletRotation = SpawnRotation + FRotator(RandomPitch, RandomYaw, 0.0f);
            
			if (WeaponItemData->WeaponData.ProjectileClass)
			{
				FActorSpawnParameters SpawnParams;
				SpawnParams.Owner = GetOwner();
				SpawnParams.Instigator = Cast<APawn>(GetOwner());
                
				// Spawn pellet with the spread rotation
				if (AProjectileBase* Pellet = GetWorld()->SpawnActor<AProjectileBase>(
					WeaponItemData->WeaponData.ProjectileClass, SpawnLocation, PelletRotation, SpawnParams))
				{
					// Use InitializeProjectile instead of InitializePelletProjectile
					Pellet->InitializeProjectile(
						WeaponItemData->WeaponData.Damage / WeaponItemData->WeaponData.ShotgunPelletCount,
						WeaponItemData->WeaponData.ProjectileSpeed,
						WeaponItemData->WeaponData.ProjectileGravityScale
					);
				}
			}
		}
	}
	else
    {
        // Single projectile (rifle, handgun, etc.)
        if (WeaponItemData->WeaponData.ProjectileClass)
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.Owner = GetOwner();
            SpawnParams.Instigator = Cast<APawn>(GetOwner());
            if (AProjectileBase* Projectile = GetWorld()->SpawnActor<AProjectileBase>(
                WeaponItemData->WeaponData.ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams))
            {
                Projectile->InitializeProjectile(
                    WeaponItemData->WeaponData.Damage,
                    WeaponItemData->WeaponData.ProjectileSpeed,
                    WeaponItemData->WeaponData.ProjectileGravityScale
                );
            }
        }
    }

    PlayGunEffects();
    StartFireCooldown();
}

void AWeaponBase::StartFireCooldown()
{
	bCanFire = false;
	float FireRate = WeaponItemData ? WeaponItemData->WeaponData.FireRate : 0.5f; // Default to 0.5 seconds if no data
	GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &AWeaponBase::ResetFireCooldown, FireRate, false);

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
	if (!bCanFire)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire failed: bCanFire is false"));
		return false;
	}
    
	if (WeaponState != EWeaponState::Equipped)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire failed: Weapon not equipped (state: %s)"), 
			   *UEnum::GetValueAsString(WeaponState));
		return false;
	}
    
	if (CurrentAmmoInMagazine <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire failed: No ammo in magazine (%d)"), CurrentAmmoInMagazine);
		return false;
	}
    
	if (bIsReloading)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire failed: Currently reloading"));
		return false;
	}
    
	if (GetWorld()->GetTimerManager().IsTimerActive(FireRateTimerHandle))
	{
		UE_LOG(LogTemp, Warning, TEXT("CanFire failed: Fire rate timer still active"));
		return false;
	}
    
	return true;
}

bool AWeaponBase::CanReload() const
{
	if (!WeaponItemData)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanReload: No weapon item data"));
		return false;
	}

	if (bIsReloading)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanReload: Already reloading"));
		return false;
	}

	if (CurrentAmmoInMagazine >= GetMaxMagazineSize())
	{
		UE_LOG(LogTemp, Warning, TEXT("CanReload: Magazine is full (%d/%d)"), CurrentAmmoInMagazine, GetMaxMagazineSize());
		return false;
	}

	bool hasReserveAmmo = CurrentReserveAmmo > 0;
	bool hasInventoryAmmo = HasAmmoInInventory();
    
	UE_LOG(LogTemp, Log, TEXT("CanReload Debug - Magazine: %d/%d, Reserve: %d, Inventory has ammo: %s, Required type: %s"), 
		CurrentAmmoInMagazine, GetMaxMagazineSize(), CurrentReserveAmmo, 
		hasInventoryAmmo ? TEXT("Yes") : TEXT("No"),
		*UEnum::GetValueAsString(WeaponItemData->WeaponData.AmmoType));

	if (!hasReserveAmmo && !hasInventoryAmmo)
	{
		UE_LOG(LogTemp, Warning, TEXT("CanReload: No ammo available in reserve (%d) or inventory"), CurrentReserveAmmo);
		return false;
	}

	return true;
}

void AWeaponBase::Reload()
{
	UE_LOG(LogTemp, Log, TEXT("Attempting to reload weapon: %s"), *WeaponItemData->ItemTextData.Name.ToString());
	if (!CanReload())
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot reload weapon: %s - either reloading is in progress or magazine is full"), 
			*WeaponItemData->ItemTextData.Name.ToString());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Reloading weapon: %s"), *WeaponItemData->ItemTextData.Name.ToString());
	bIsReloading = true;

	// Play reload animation montage if available
	if (WeaponItemData && WeaponItemData->WeaponData.ReloadMontage)
	{
		// Get the character that owns this weapon
		if (APawn* OwningPawn = Cast<APawn>(GetOwner()))
		{
			if (USkeletalMeshComponent* OwnerMesh = OwningPawn->FindComponentByClass<USkeletalMeshComponent>())
			{
				if (UAnimInstance* AnimInstance = OwnerMesh->GetAnimInstance())
				{
					float MontageDuration = AnimInstance->Montage_Play(WeaponItemData->WeaponData.ReloadMontage, 1.0f);
					UE_LOG(LogTemp, Log, TEXT("Playing reload montage for weapon: %s, duration: %f"), 
						*WeaponItemData->ItemTextData.Name.ToString(), MontageDuration);
				}
			}
		}
	}
	// Calculate how much ammo we need
	int32 AmmoNeeded = GetMaxMagazineSize() - CurrentAmmoInMagazine;
	
	// Try to get ammo from inventory first
	int32 AmmoFromInventory = GetAmmoFromInventory(AmmoNeeded);
	
	if (AmmoFromInventory > 0)
	{
		// Use ammo from inventory
		CurrentAmmoInMagazine += AmmoFromInventory;
		UE_LOG(LogTemp, Log, TEXT("Reloaded %d rounds from inventory"), AmmoFromInventory);
	}
	else if (CurrentReserveAmmo > 0)
	{
		// Fallback to reserve ammo
		int32 AmmoToReload = FMath::Min(AmmoNeeded, CurrentReserveAmmo);
		CurrentAmmoInMagazine += AmmoToReload;
		CurrentReserveAmmo -= AmmoToReload;
		UE_LOG(LogTemp, Log, TEXT("Reloaded %d rounds from reserve"), AmmoToReload);
	}

	// Start reload timer
	if (WeaponItemData)
	{
		GetWorld()->GetTimerManager().SetTimer(
			ReloadTimerHandle,  // Use separate timer handle
			[this]() { bIsReloading = false; },
			WeaponItemData->WeaponData.ReloadTime,
			false
		);
	}
}

void AWeaponBase::SetWeaponState(EWeaponState NewState)
{
	WeaponState = NewState;
	UE_LOG(LogTemp, Log, TEXT("Weapon state changed to: %s"), *UEnum::GetValueAsString(NewState));
	// Enable firing when weapon is equipped
	if (WeaponState == EWeaponState::Equipped)
	{
		
		bCanFire = true;
		UE_LOG(LogTemp, Log, TEXT("Weapon %s is now equipped and can fire"), *GetNameSafe(this));
	}
	else
	{
		bCanFire = false;
		UE_LOG(LogTemp, Log, TEXT("Weapon %s is no longer equipped and cannot fire"), *GetNameSafe(this));
	}

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


bool AWeaponBase::HasAmmoInInventory() const
{
	if (!OwningCharacter || !WeaponItemData) 
	{
		UE_LOG(LogTemp, Warning, TEXT("HasAmmoInInventory: Missing character or weapon data"));
		return false;
	}

	UInventoryComponent* Inventory = OwningCharacter->GetInventory();
	if (!Inventory) 
	{
		UE_LOG(LogTemp, Warning, TEXT("HasAmmoInInventory: No inventory component"));
		return false;
	}

	EAmmoType RequiredAmmoType = WeaponItemData->WeaponData.AmmoType;
    
	// Search for matching ammo type in inventory
	for (UItemBase* Item : Inventory->GetInventoryContents())
	{
		if (Item && Item->ItemType == EItemType::Ammo)
		{
			UE_LOG(LogTemp, Log, TEXT("Found ammo item: Type=%s, Required=%s, Quantity=%d"), 
				*UEnum::GetValueAsString(Item->AmmoData.AmmoType),
				*UEnum::GetValueAsString(RequiredAmmoType),
				Item->Quantity);
                
			if (Item->AmmoData.AmmoType == RequiredAmmoType && Item->Quantity > 0)
			{
				return true;
			}
		}
	}
	return false;
}

int32 AWeaponBase::GetAmmoFromInventory(int32 AmountNeeded)
{
	if (!OwningCharacter || !WeaponItemData) return 0;

	UInventoryComponent* Inventory = OwningCharacter->GetInventory();
	if (!Inventory) return 0;

	EAmmoType RequiredAmmoType = WeaponItemData->WeaponData.AmmoType;
	int32 AmmoRetrieved = 0;

	// Create a copy to iterate over since we'll be modifying the original
	TArray<UItemBase*> InventoryItems = Inventory->GetInventoryContents();

    
	for (UItemBase* Item : InventoryItems)
	{
		if (Item && Item->ItemType == EItemType::Ammo && 
			Item->AmmoData.AmmoType == RequiredAmmoType)
		{
			int32 AmmoToTake = FMath::Min(AmountNeeded - AmmoRetrieved, Item->Quantity);
			AmmoRetrieved += AmmoToTake;
            
			// Actually remove the ammo from inventory
			if (AmmoToTake >= Item->Quantity)
			{
				// Remove entire stack
				Inventory->RemoveSingleInstanceOfItem(Item);
			}
			else
			{
				// Remove partial amount
				Inventory->RemoveAmountOfItem(Item, AmmoToTake);
			}
            
			if (AmmoRetrieved >= AmountNeeded) break;
		}
	}
    
	return AmmoRetrieved;
}

EAmmoType AWeaponBase::GetRequiredAmmoType() const
{
	if (!WeaponItemData) return EAmmoType::PistolAmmo;
	return WeaponItemData->WeaponData.AmmoType;
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

