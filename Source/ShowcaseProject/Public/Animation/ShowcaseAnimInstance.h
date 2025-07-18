// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Components/WeaponSystemComponent/WeaponSystemComponent.h"
#include "Data/ST_ItemDataStructs.h"
#include "ShowcaseAnimInstance.generated.h"

class UCharacterMovementComponent;
class AShowcaseProjectCharacter;
class AWeaponBase;
enum class EWeaponSlot : uint8;
/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UShowcaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, Category="Weapon Animation")
	bool bIsEquippingPrimary = false;

	UPROPERTY(BlueprintReadOnly, Category="Weapon Animation")
	EWeaponSlot WeaponToEquip = EWeaponSlot::Primary;

public:
	// Used in animation blueprint
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);
	void CheckEquippedWeapon();
	virtual void NativeInitializeAnimation() override;

private:
	// Reference to the owning character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	AShowcaseProjectCharacter* Character;

	// Reference to the character movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	UCharacterMovementComponent* CharacterMovement;
	
	//Ground Speed and Velocity
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	FVector Velocity;

	//Direction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bShouldMove;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bIsFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon Animation", meta=(AllowPrivateAccess="true"))
	bool bIsShotgunEquipped = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon Animation", meta=(AllowPrivateAccess="true"))
	bool bIsRifleEquipped = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon Animation", meta=(AllowPrivateAccess="true"))
	bool bIsHandgunEquipped = false;
	
};
