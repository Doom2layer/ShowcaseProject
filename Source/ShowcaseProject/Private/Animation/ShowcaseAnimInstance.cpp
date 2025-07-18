// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ShowcaseAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Components/WeaponSystemComponent/WeaponSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/ItemBase.h"
#include "Weapons/WeaponBase.h"
#include "KismetAnimationLibrary.h"
#include "Player/ShowcaseProjectCharacter.h"

// Act like beginplay
void UShowcaseAnimInstance::NativeInitializeAnimation()
{
	AShowcaseProjectCharacter* OwningCharacter = Cast<AShowcaseProjectCharacter>(TryGetPawnOwner());
	CharacterMovement = OwningCharacter ? OwningCharacter->GetCharacterMovement() : nullptr;
}


//Act like tick
void UShowcaseAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Character == nullptr)
	{
		Character = Cast<AShowcaseProjectCharacter>(TryGetPawnOwner());
	}
	if (Character)
	{
		Velocity = CharacterMovement->Velocity;
		GroundSpeed = Velocity.Size();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
		bShouldMove = GroundSpeed > 3.0f && CharacterMovement->GetCurrentAcceleration().Size() > 0.0f;
		bIsFalling = CharacterMovement->IsFalling();
		if (Character->GetWeaponSystem())
		{
			CheckEquippedWeapon();
		}
		else
		{
			bIsHandgunEquipped = false;
			bIsRifleEquipped = false;
			bIsShotgunEquipped = false;
			
		}
	}
}

void UShowcaseAnimInstance::CheckEquippedWeapon()
{
	bIsHandgunEquipped = false;
	bIsRifleEquipped = false;
	bIsShotgunEquipped = false;

	if (Character && Character->GetWeaponSystem())
	{
		AWeaponBase* EquippedWeapon = Character->GetWeaponSystem()->GetEquippedWeapon();
		if (EquippedWeapon)
		{
			switch (EquippedWeapon->GetWeaponItemData()->WeaponCategory)
			{
			case EWeaponCategory::Handgun:
				bIsHandgunEquipped = true;
				break;
			case EWeaponCategory::Rifle:
				bIsRifleEquipped = true;
				break;
			case EWeaponCategory::Shotgun:
				bIsShotgunEquipped = true;
				break;
			default:
				break;
			}
		}else
		{
			// No weapon equipped
			bIsHandgunEquipped = false;
			bIsRifleEquipped = false;
			bIsShotgunEquipped = false;
		}
	}
}

