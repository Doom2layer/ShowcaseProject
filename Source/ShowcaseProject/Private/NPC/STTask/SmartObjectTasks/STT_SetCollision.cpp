// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC/STTask/SmartObjectTasks/STT_SetCollision.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "NPC/Character/NPC_BaseCharacter.h"


const UStruct* FSTT_SetCollision::GetInstanceDataType() const
{
	return FSTT_SetCollisionInstanceData::StaticStruct();
}

EStateTreeRunStatus FSTT_SetCollision::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	if (!InstanceData.Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor is not valid!"));
		return EStateTreeRunStatus::Failed;
	}

	if (ANPC_BaseCharacter* Character = Cast<ANPC_BaseCharacter>(InstanceData.Actor))
	{
		UE_LOG(LogTemp, Log, TEXT("Setting collision response for %s to %s on channel %s"),
			*Character->GetCapsuleComponent()->GetName(),
			*UEnum::GetValueAsString(InstanceData.NewResponse),
			*UEnum::GetValueAsString(InstanceData.CollisionChannel));
		
		InstanceData.OriginalResponse = Character->GetCapsuleComponent()->GetCollisionResponseToChannel(InstanceData.CollisionChannel);
		Character->GetCapsuleComponent()->SetCollisionResponseToChannel(InstanceData.CollisionChannel, InstanceData.NewResponse);

		return EStateTreeRunStatus::Succeeded;
	}
		return EStateTreeRunStatus::Failed;
}

void FSTT_SetCollision::ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	// set the collision response back to the original value
	if (InstanceData.Actor)
	{
		if (ACharacter* Character = Cast<ACharacter>(InstanceData.Actor))
		{
			if (Character->GetCapsuleComponent())
			{
				Character->GetCapsuleComponent()->SetCollisionResponseToChannel(InstanceData.CollisionChannel, InstanceData.OriginalResponse);
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Character does not have a CapsuleComponent!"));
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Actor is not a Character!"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor is not valid!"));
	}
	
}
