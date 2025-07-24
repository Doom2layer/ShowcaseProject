// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UProjectileMovementComponent;

UCLASS()
class SHOWCASEPROJECT_API AProjectileBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectileBase();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void SetDamageInstigator(AController* NewInstigator) { DamageInstigator = NewInstigator; }

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void SetDamageSource(AActor* NewSource) { DamageSource = NewSource; }

	void InitializeProjectile(float Damage, float Speed, float GravityScale);

	void InitializePelletProjectile(float Damage, float Speed, float GravityScale, int32 Pellets, float Spread);

	void DestroyProjectile();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	bool bIsPelletProjectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	int32 PelletCount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float SpreadAngle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	float ProjectileDamage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile|Lifetime")
	float ProjectileLifetime = 10.0f; // Default 10 seconds

	FTimerHandle LifetimeTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	AController* DamageInstigator;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	AActor* DamageSource;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float HeadshotMultiplier = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	TArray<FName> HeadshotBones = {TEXT("head"), TEXT("Head"), TEXT("skull"), TEXT("Skull")};

	virtual void ApplyDamageToTarget(AActor* Target, const FHitResult& HitResult);
	virtual bool IsHeadshotHit(const FHitResult& HitResult) const;
	virtual float CalculateDamageForTarget(AActor* Target, const FHitResult& HitResult) const;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,FVector NormalImpulse, const FHitResult& Hit);

};
