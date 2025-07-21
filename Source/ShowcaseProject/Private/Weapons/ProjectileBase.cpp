// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapons/ProjectileBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AProjectileBase::AProjectileBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Create collision component
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->SetSphereRadius(5.0f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	CollisionComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
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

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                            FVector NormalImpulse, const FHitResult& Hit)
{
	// Ignore collisions with other projectiles and the owner
	if (OtherActor && OtherActor != GetOwner() && !OtherActor->IsA<AProjectileBase>())
	{
		if (GetWorldTimerManager().IsTimerActive(LifetimeTimerHandle))
		{
			GetWorldTimerManager().ClearTimer(LifetimeTimerHandle);
		}
		UE_LOG(LogTemp, Log, TEXT("Projectile hit: %s"), *OtherActor->GetName());
		Destroy();
	}
}

// Called every frame
void AProjectileBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

