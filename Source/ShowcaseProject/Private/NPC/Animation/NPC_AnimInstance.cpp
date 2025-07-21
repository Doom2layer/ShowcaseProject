// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Animation/NPC_AnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "KismetAnimationLibrary.h"
#include "NPC/Character/NPC_BaseCharacter.h"

void UNPC_AnimInstance::NativeInitializeAnimation()
{
	ANPC_BaseCharacter* OwningNPC = Cast<ANPC_BaseCharacter>(TryGetPawnOwner());
	CharacterMovement = OwningNPC ? OwningNPC->GetCharacterMovement() : nullptr;
}

void UNPC_AnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (Character == nullptr)
	{
		Character = Cast<ANPC_BaseCharacter>(TryGetPawnOwner());
	}
	if (Character)
	{
		Velocity = CharacterMovement->Velocity;
		GroundSpeed = Velocity.Size();
		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Character->GetActorRotation());
		bShouldMove = GroundSpeed > 0.0f;
		bIsFalling = CharacterMovement->IsFalling();
	}
}
