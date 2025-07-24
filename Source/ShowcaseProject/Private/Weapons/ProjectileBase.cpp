// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Interfaces/DamageableInterface.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetSphereRadius(5.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); // For accurate hit detection

	// Make sure projectile doesn't collide with shooter
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block); // Re-enable after spawn
	
	RootComponent = CollisionComponent;

	// Create mesh component
	ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ProjectileMesh"));
	ProjectileMesh->SetupAttachment(RootComponent);
	ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Create projectile movement component
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 3000.0f;
	ProjectileMovement->MaxSpeed = 3000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

	// Bind hit event
	CollisionComponent->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);

	ProjectileDamage = 25.0f;
}

// Called when the game starts or when spawned
void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	if (ProjectileLifetime > 0.0f)
	{
		GetWorldTimerManager().SetTimer(
		LifetimeTimerHandle,
		this,
		&AProjectileBase::DestroyProjectile,
		ProjectileLifetime,
		false // Do not loop
		);
	}
}

void AProjectileBase::InitializeProjectile(float Damage, float Speed, float GravityScale)
{
	ProjectileDamage = Damage;
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->ProjectileGravityScale = GravityScale;
}

void AProjectileBase::InitializePelletProjectile(float Damage, float Speed, float GravityScale, int32 Pellets,
	float Spread)
{
	ProjectileDamage = Damage / Pellets; // Divide damage among pellets
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;
	ProjectileMovement->ProjectileGravityScale = GravityScale;
	bIsPelletProjectile = true;
	PelletCount = Pellets;
	SpreadAngle = Spread;

	// Apply random spread to the direction
	FVector ForwardDirection = GetActorForwardVector();
	FRotator SpreadRotation = FRotator(
		FMath::RandRange(-SpreadAngle, SpreadAngle),
		FMath::RandRange(-SpreadAngle, SpreadAngle),
		0.0f
	);
	FVector AdjustedDirection = SpreadRotation.RotateVector(ForwardDirection);
	ProjectileMovement->Velocity = AdjustedDirection * Speed;

}

void AProjectileBase::DestroyProjectile()
{
	UE_LOG(LogTemp, Log, TEXT("Projectile destroyed after lifetime: %f seconds"), ProjectileLifetime);
    
	// Clear the timer handle
	if (GetWorldTimerManager().IsTimerActive(LifetimeTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(LifetimeTimerHandle);
	}
    
	// Destroy the projectile
	Destroy();
}

void AProjectileBase::ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult)
{
	if (!Target || !Target->Implements<UDamageableInterface>())
	{
		return;
	}

	float FinalDamage = CalculateDamageForTarget(Target, HitResult);
	bool bIsHeadshot = IsHeadshotHit(HitResult);

	// Create custom projectile damage event
	FProjectileDamageEvent DamageEvent(FinalDamage, HitResult.Location, GetVelocity().GetSafeNormal(),
									 HitResult.Component.Get(), HitResult.BoneName);
	DamageEvent.ProjectileSpeed = GetVelocity().Size();
	DamageEvent.bIsHeadshot = bIsHeadshot;

	// Apply damage through interface
	IDamageableInterface* DamageableTarget = Cast<IDamageableInterface>(Target);
	if (DamageableTarget)
	{
		float ActualDamage = DamageableTarget->TakeDamage(FinalDamage, DamageEvent, DamageInstigator, DamageSource ? DamageSource : GetOwner());
        
		UE_LOG(LogTemp, Log, TEXT("Projectile dealt %f damage to %s %s"), 
			   ActualDamage, *Target->GetName(), bIsHeadshot ? TEXT("(HEADSHOT)") : TEXT(""));
	}

}

bool AProjectileBase::IsHeadshotHit(const FHitResult& HitResult) const
{
	if (HitResult.BoneName == NAME_None)
	{
		return false;
	}

	for (const FName& HeadshotBone : HeadshotBones)
	{
		if (HitResult.BoneName.ToString().Contains(HeadshotBone.ToString()))
		{
			return true;
		}
	}
	return false;

}

float AProjectileBase::CalculateDamageForTarget(AActor* Target, const FHitResult& HitResult) const
{
	float BaseDamage = ProjectileDamage;
    
	// Apply headshot multiplier
	if (IsHeadshotHit(HitResult))
	{
		BaseDamage *= HeadshotMultiplier;
	}

	// Let the target modify incoming damage (armor, resistances, etc.)
	if (IDamageableInterface* DamageableTarget = Cast<IDamageableInterface>(Target))
	{
		FProjectileDamageEvent TempEvent(BaseDamage, HitResult.Location, GetVelocity().GetSafeNormal(),
									   HitResult.Component.Get(), HitResult.BoneName);
		BaseDamage = DamageableTarget->ModifyIncomingDamage(BaseDamage, TempEvent, DamageInstigator, DamageSource);
	}

	return BaseDamage;
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == GetOwner() || OtherActor->IsA<AProjectileBase>())
	{
		UE_LOG(LogTemp, Warning, TEXT("Projectile hit ignored - invalid target: %s"), *GetNameSafe(OtherActor));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Projectile hit: %s at location %s, bone: %s"), 
		   *OtherActor->GetName(), 
		   *Hit.Location.ToString(),
		   *Hit.BoneName.ToString());

	// Clear lifetime timer
	if (GetWorldTimerManager().IsTimerActive(LifetimeTimerHandle))
	{
		GetWorldTimerManager().ClearTimer(LifetimeTimerHandle);
	}

	// Check if target implements damage interface
	if (OtherActor->GetClass()->ImplementsInterface(UDamageableInterface::StaticClass()))
	{
		UE_LOG(LogTemp, Log, TEXT("Target %s implements DamageableInterface - applying damage"), *OtherActor->GetName());
		ApplyDamageToTarget(OtherActor, Hit);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Target %s does not implement DamageableInterface"), *OtherActor->GetName());
	}

	Destroy();
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

