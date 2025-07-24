// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/DamageableInterface.h"

// Add default functionality here for any IDamageableInterface functions that are not pure virtual.

float IDamageableInterface::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser)
{
	return 0.0f; // Default implementation, can be overridden
}

float IDamageableInterface::GetHealth() const
{
	return 0.0f; // Default implementation, can be overridden
}

float IDamageableInterface::GetMaxHealth() const
{
	return 0.0f; // Default implementation, can be overridden
}

void IDamageableInterface::Die(const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
}

