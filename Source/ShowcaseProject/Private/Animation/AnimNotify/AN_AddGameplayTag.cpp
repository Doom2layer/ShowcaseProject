// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/AnimNotify/AN_AddGameplayTag.h"

#include "Interfaces/InteractionInterface.h"

void UAN_AddGameplayTag::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp->GetOwner()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
	{
		Target = MeshComp->GetOwner();
		Target->SetGameplayTag(GameplayTagToAdd);
	}
}
