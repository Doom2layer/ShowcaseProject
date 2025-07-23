// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShowcaseHUD.generated.h"

struct FDialogueNode;
class UMainMenu;
struct FInteractableData;
class UInteractionWidget;
class UInventoryMenu;
class UWeaponHUD;
class AWeaponBase;
class UDialogueWidget;
class UDialogueComponent;

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

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UWeaponHUD> WeaponHUDClass;

	UPROPERTY(EditDefaultsOnly, Category = "Widgets")
	TSubclassOf<UDialogueWidget> DialogueWidgetClass;
	
	bool bIsInventoryMenuVisible;

	bool bIsMainMenuVisible;

	//FUNCTIONS
	AShowcaseHUD();
	FORCEINLINE UWeaponHUD* GetWeaponHUD() const { return WeaponHUDWidget; }
	void ToggleInventoryMenu();
	void ToggleMainMenu();
	void ShowInteractionWidget() const;
	void HideInteractionWidget() const;
	void UpdateInteractionWidget(const FInteractableData* InteractableData) const;
	void UpdateWeaponDisplay(AWeaponBase* EquippedWeapon);
	void OnWeaponAimStart();
	void OnWeaponAimStop();
	void OnWeaponFired();
	void OnWeaponStoppedFiring();

	UFUNCTION(BlueprintCallable, Category="HUD | Dialogue")
	void ShowDialogue(const FDialogueNode& DialogueNode, UDialogueComponent* DialogueComponent);

	UFUNCTION(BlueprintCallable, Category="HUD | Dialogue")
	void HideDialogue();
	
protected:

	//PROPERTIES
	UPROPERTY()
	UMainMenu* MainMenuWidget;
	
	UPROPERTY()
	UInventoryMenu* InventoryMenuWidget;

	UPROPERTY()
	UWeaponHUD* WeaponHUDWidget;

	UPROPERTY()
	UDialogueWidget* DialogueWidget;
	
	UPROPERTY()
	UInteractionWidget* InteractionWidget;

	UPROPERTY()
	FTimerHandle TimeDilationTimerHandle;
	
	UPROPERTY()
	float CurrentTimeDilation = 1.0f;
	
	UPROPERTY()
	float TargetTimeDilation = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category="Time Dilation")
	float TimeDilationInterpSpeed = 3.0f;

	//FUNCTIONS
	virtual void BeginPlay() override;
	void UpdateTimeDilation();
};
