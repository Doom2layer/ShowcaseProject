// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ShowcaseAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "Components/WeaponSystemComponent/WeaponSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Items/ItemBase.h"
#include "Weapons/WeaponBase.h"
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
	}
}

void UShowcaseAnimInstance::CheckEquippedWeapon()
{
	if (Character && Character->GetWeaponSystem())
	{
		if (AWeaponBase* EquippedWeapon = Character->GetWeaponSystem()->GetEquippedWeapon())
		{
			switch (EquippedWeapon->GetWeaponItemData()->WeaponCategory)
			{
			case EWeaponCategory::Handgun:
				bIsHandgunEquipped = true;
				bIsRifleEquipped = false;
				bIsShotgunEquipped = false;
				break;
			case EWeaponCategory::Rifle:
				bIsHandgunEquipped = false;
				bIsRifleEquipped = true;
				bIsShotgunEquipped = false;
				break;
			case EWeaponCategory::Shotgun:
				bIsHandgunEquipped = false;
				bIsRifleEquipped = false;
				bIsShotgunEquipped = true;
				break;
			default:
				break;
			}
		}
		else
		{
			// No weapon equipped
			bIsHandgunEquipped = false;
			bIsRifleEquipped = false;
			bIsShotgunEquipped = false;
		}
	}
}

