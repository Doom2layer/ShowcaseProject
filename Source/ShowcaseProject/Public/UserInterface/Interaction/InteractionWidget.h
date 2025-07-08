// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InteractionWidget.generated.h"

class UProgressBar;
struct FInteractableData;
class AShowcaseProjectCharacter;
class UTextBlock;
/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UInteractionWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	//Properties
	UPROPERTY(VisibleAnywhere, Category="Interaction Widget | Player Reference")
	AShowcaseProjectCharacter* PlayerReference;

	//Functions
	void UpdateWidget(const FInteractableData* InteractableData) const;

protected:
	//Properties
	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category= "Interaction Widget | Interactable Data")
	UTextBlock* NameText;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category= "Interaction Widget | Interactable Data")
	UTextBlock* ActionText;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category= "Interaction Widget | Interactable Data")
	UTextBlock* QuantityText;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category= "Interaction Widget | Interactable Data")
	UTextBlock* KeyPressText;

	UPROPERTY(VisibleAnywhere, meta=(BindWidget), Category= "Interaction Widget | Interactable Data")
	UProgressBar* InteractionProgressBar;

	float CurrentInteractionDuration;

	//Functions

	UFUNCTION(Category= "Interaction Widget | Interactable Data")
	float UpdateInteractionProgressBar();

	virtual void NativeOnInitialized() override;

	virtual void NativeConstruct() override;
};
