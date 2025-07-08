// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryMenu.generated.h"

class AShowcaseProjectCharacter;
/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UInventoryMenu : public UUserWidget
{
	GENERATED_BODY()



public:
	UPROPERTY()
	AShowcaseProjectCharacter* PlayerCharacter;

protected:
	virtual void NativeOnInitialized() override;
	virtual void NativeConstruct() override;
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;
};
