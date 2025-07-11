// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryContextMenu.generated.h"

class AShowcaseProjectCharacter;
class UItemBase;
class UButton;
/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UInventoryContextMenu : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(meta=(BindWidget))
	UButton* UseButton;
	UPROPERTY(meta=(BindWidget))
	UButton* AssignPrimaryButton;
	UPROPERTY(meta=(BindWidget))
	UButton* AssignSecondaryButton;
	UPROPERTY(meta=(BindWidget))
	UButton* EquipButton;
	UPROPERTY(meta=(BindWidget))
	UButton* ExamineButton;
	UPROPERTY(meta=(BindWidget))
	UButton* DiscardButton;
	UPROPERTY(meta=(BindWidget))
	UButton* CombineButton;

	UPROPERTY(BlueprintReadWrite)
	UItemBase* ItemReference;

	UPROPERTY(BlueprintReadWrite)
	AShowcaseProjectCharacter* PlayerCharacter;

protected:
	virtual void NativeConstruct() override;
	
private:
	UFUNCTION()
	void OnUseButtonClicked();
	UFUNCTION()
	void OnAssignPrimaryButtonClicked();
	UFUNCTION()
	void OnAssignSecondaryButtonClicked();
	UFUNCTION()
	void OnExamineButtonClicked();
	UFUNCTION()
	void OnDiscardButtonClicked();
	UFUNCTION()
	void OnCombineButtonClicked();
	UFUNCTION()
	void OnEquipButtonClicked();
	void SetupButtonsForItemType();
};
