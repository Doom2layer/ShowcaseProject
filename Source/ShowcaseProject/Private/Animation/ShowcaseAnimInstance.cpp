// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/ShowcaseAnimInstance.h"
#include "Components/WeaponSystemComponent/WeaponSystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
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
		bShouldMove = GroundSpeed > 3.0f && CharacterMovement->GetCurrentAcceleration().Size() > 0.0f;
		bIsFalling = CharacterMovement->IsFalling();
	}
}

