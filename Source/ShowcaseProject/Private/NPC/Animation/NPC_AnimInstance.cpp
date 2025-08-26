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
		if (Character->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
		{
			TargetInteractable = Character;
			FGameplayTagContainer Tags = TargetInteractable->GetGameplayTags();
			if (Tags.HasTag(FGameplayTag::RequestGameplayTag(TEXT("SmartObject.Interaction.Sit"))))
			{
				bIsSitting = true;
			}
			else
			{
				bIsSitting = false;
			}
		}

	}

	// Displacement since last frame 
	if (IsValid(Character))
	{
		const FVector CurrentLocation = Character->GetActorLocation();
		DisplacementAmount = (CurrentLocation - WorldLocation).Size2D();
		WorldLocation = CurrentLocation;
	}
	else
	{
		DisplacementAmount = 0.0f;
		WorldLocation = FVector::ZeroVector;
	}

	// Distance / by DeltaTime to get rate of change over time
	if (DeltaTime > 0.0f)
	{
		DisplacementSpeed = DisplacementAmount / DeltaTime;
	}
	else
	{
		DisplacementSpeed = 0.0f;
	}

	// Ignore first frame since world location will be zero
	if (bIsFirstFrame)
	{
		bIsFirstFrame = false;
		DisplacementSpeed = 0.0f;
		DisplacementAmount = 0.0f;
	}
	
}
