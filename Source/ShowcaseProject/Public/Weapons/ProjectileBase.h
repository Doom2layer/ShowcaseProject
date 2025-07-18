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

	UFUNCTION(BlueprintCallable)
	void InitializeProjectile(float Damage, float Speed, float GravityScale);

	UFUNCTION(BlueprintCallable)
	void InitializePelletProjectile(float Damage, float Speed, float GravityScale, int32 Pellets, float Spread);
	
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
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	// USphereComponent* CollisionComponent;
	//
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	// UStaticMeshComponent* ProjectileMesh;
	//
	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	// UProjectileMovementComponent* ProjectileMovement;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	// float ProjectileDamage;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
