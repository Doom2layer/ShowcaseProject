// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Engine/DamageEvents.h"
#include "DamageableInterface.generated.h"

class AController;
struct FDamageEvent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamageTaken, float, DamageAmount, AActor*, DamageCauser, const FDamageEvent&, DamageEventInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDeath, AActor*, Killer, const FDamageEvent&, DamageEventInfo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChanged, float, Health, float, MaxHealth);

USTRUCT()
struct FProjectileDamageEvent : public FDamageEvent
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FVector HitLocation;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FVector HitDirection;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	UPrimitiveComponent* HitComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	FName BoneName;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	float ProjectileSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Damage")
	bool bIsHeadshot;

	FProjectileDamageEvent()
		: FDamageEvent()
	, HitLocation(FVector::ZeroVector)
	, HitDirection(FVector::ZeroVector)
	, HitComponent(nullptr)
	, BoneName(NAME_None)
	, ProjectileSpeed(0.0f)
	, bIsHeadshot(false)
	{
		DamageTypeClass = UDamageType::StaticClass();
	}

	FProjectileDamageEvent(float InDamage, const FVector& InHitLocation, const FVector& InHitDirection, 
					  UPrimitiveComponent* InHitComponent, const FName& InBoneName = NAME_None)
	: FDamageEvent()
	, HitLocation(InHitLocation)
	, HitDirection(InHitDirection)
	, HitComponent(InHitComponent)
	, BoneName(InBoneName)
	, ProjectileSpeed(0.0f)
	, bIsHeadshot(false)
	{
		DamageTypeClass = UDamageType::StaticClass();
	}
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UDamageableInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SHOWCASEPROJECT_API IDamageableInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Core damage interface
	virtual float TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// Health status queries
	virtual float GetHealth() const;
	virtual float GetMaxHealth() const;
	virtual bool IsAlive() const { return GetHealth() > 0.0f; }

	// Death handling
	virtual void Die(const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser);

	// Damage resistance/immunity system
	virtual bool CanTakeDamage(const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const { return true; }
	virtual float ModifyIncomingDamage(float BaseDamage, const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser) const { return BaseDamage; }

	// Damage type specific handling
	virtual float GetDamageMultiplier(const FDamageEvent& DamageEvent, const FName& BoneName = NAME_None) const { return 1.0f; }
	virtual bool ShouldRagdollOnDeath() const { return false; }
	virtual bool ShouldPlayDeathAnimation() const { return true; }

	// Blueprint accessible events
	UFUNCTION(BlueprintImplementableEvent, Category = "Damage")
	void OnDamageReceived(float DamageAmount, AActor* DamageCauser, const FVector& HitLocation);
    
	UFUNCTION(BlueprintImplementableEvent, Category = "Damage")
	void OnActorDied(AActor* Killer, const FVector& DeathLocation);
};