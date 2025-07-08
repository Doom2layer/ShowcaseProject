// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShowcaseHUD.generated.h"

class UMainMenu;
struct FInteractableData;
class UInteractionWidget;
class UInventoryMenu;
/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API AShowcaseHUD : public AHUD
{
	GENERATED_BODY()

public:
	//PROPERTIES
	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UMainMenu> MainMenuWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UInventoryMenu> InventoryMenuClass;
	
	UPROPERTY(EditDefaultsOnly, Category="Widgets")
	TSubclassOf<UInteractionWidget> InteractionWidgetClass;

	
	
	bool bIsInventoryMenuVisible;

	bool bIsMainMenuVisible;

	//FUNCTIONS
	AShowcaseHUD();

	void ToggleInventoryMenu();
	void ToggleMainMenu();
	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;

protected:

	//PROPERTIES
	UPROPERTY()
	UMainMenu* MainMenuWidget;
	
	UPROPERTY()
	UInventoryMenu* InventoryMenuWidget;

	UPROPERTY()
	UInteractionWidget* InteractionWidget;

	//FUNCTIONS
	virtual void BeginPlay() override;
};
