// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "DialogueWidget.generated.h"

class UChoiceButton;
class UDialogueComponent;
class UVerticalBox;
class UTextBlock;
class UButton;

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnChoiceSelectedDelegate, int32, ChoiceIndex);

UCLASS()
class SHOWCASEPROJECT_API UDialogueWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void DisplayDialogueNode(const FDialogueNode& DialogueNode);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void HideDialogueNode();

	UFUNCTION(BlueprintImplementableEvent, Category = "Dialogue")
	void OnDialogueDisplayed(const FDialogueNode& DialogueNode);

	// Set the dialogue component reference
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SetDialogueComponent(UDialogueComponent* InDialogueComponent);

	// Choice selection delegate
	UPROPERTY(BlueprintAssignable, Category = "Dialogue")
	FOnChoiceSelectedDelegate OnChoiceSelected;

protected:
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* SpeakerName;

	UPROPERTY(meta=(BindWidget))
	UTextBlock* DialogueText;

	UPROPERTY(meta=(BindWidget))
	UVerticalBox* ChoicesContainer;

private:
	// Critical: Use weak pointer for cross-system references
	UPROPERTY()
	TWeakObjectPtr<UDialogueComponent> WeakDialogueComponent;
    
	// State tracking for validation
	bool bIsDialogueComponentBound = false;

	UFUNCTION()
	void HandleChoiceButtonClicked(UChoiceButton* ClickedButton, int32 ChoiceIndex);
	
	void CleanupDialogueBinding();
	
	void CleanupChoiceButtons();
    
	TArray<UChoiceButton*> ChoiceButtons;
};
