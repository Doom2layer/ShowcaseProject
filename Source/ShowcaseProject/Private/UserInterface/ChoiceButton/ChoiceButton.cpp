// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/ChoiceButton/ChoiceButton.h"

UChoiceButton::UChoiceButton()
{
	ChoiceIndex = -1;
	bIsInitialized = false;
    
	// Bind directly in constructor - UButton's OnClicked is available immediately
	OnClicked.AddDynamic(this, &UChoiceButton::HandleInternalClick);
	bIsInitialized = true;
    
	UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Constructor binding completed"));

}

void UChoiceButton::SetChoiceIndex(int32 InChoiceIndex)
{
	ChoiceIndex = InChoiceIndex;
	UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Index set to %d"), ChoiceIndex);
}

void UChoiceButton::InitializeChoiceButton(int32 InChoiceIndex)
{
	SetChoiceIndex(InChoiceIndex);
	UE_LOG(LogTemp, Log, TEXT("ChoiceButton: Initialized with index %d"), InChoiceIndex);
}

void UChoiceButton::HandleInternalClick()
{
	if (ChoiceIndex >= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("ChoiceButton[%d]: Self-identifying click"), ChoiceIndex);
		OnChoiceButtonClicked.Broadcast(this, ChoiceIndex);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("ChoiceButton: Invalid choice index during click"));
	}
}
