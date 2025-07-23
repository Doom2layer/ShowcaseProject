// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Button.h"
#include "ChoiceButton.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnChoiceButtonClickedDelegate, class UChoiceButton*, ClickedButton, int32, ChoiceIndex);


/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UChoiceButton : public UButton
{
	GENERATED_BODY()
	
public:
	UChoiceButton();

	// Self-identification delegate
	UPROPERTY(BlueprintAssignable, Category = "Choice")
	FOnChoiceButtonClickedDelegate OnChoiceButtonClicked;

	UFUNCTION(BlueprintCallable, Category = "Choice")
	void SetChoiceIndex(int32 InChoiceIndex);

	UFUNCTION(BlueprintPure, Category = "Choice")
	int32 GetChoiceIndex() const { return ChoiceIndex; }

	// Manual initialization after widget tree construction
	UFUNCTION(BlueprintCallable, Category = "Choice")
	void InitializeChoiceButton(int32 InChoiceIndex);

private:
	UPROPERTY()
	int32 ChoiceIndex = -1;

	UPROPERTY()
	bool bIsInitialized = false;

	UFUNCTION()
	void HandleInternalClick();
};
