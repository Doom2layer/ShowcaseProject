// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/InteractionInterface.h"

// Add default functionality here for any IInteractionInterface functions that are not pure virtual.

void IInteractionInterface::BeginFocus()
{
}

void IInteractionInterface::EndFocus()
{
}

void IInteractionInterface::BeginInteract()
{
}

void IInteractionInterface::EndInteract()
{
}

void IInteractionInterface::Interact(AShowcaseProjectCharacter* PlayerCharacter)
{
}

FGameplayTagContainer IInteractionInterface::GetGameplayTags() const
{
	return FGameplayTagContainer();
}

void IInteractionInterface::SetGameplayTag(const FGameplayTag& Tag)
{
}

void IInteractionInterface::RemoveGameplayTag(const FGameplayTag& Tag)
{
}