// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/InventoryMenu/InventoryMenu.h"
#include "Player/ShowcaseProjectCharacter.h"

void UInventoryMenu::NativeOnInitialized()
{
	Super::NativeOnInitialized();
}

void UInventoryMenu::NativeConstruct()
{
	Super::NativeConstruct();

	PlayerCharacter = Cast<AShowcaseProjectCharacter>(GetOwningPlayerPawn());
}

