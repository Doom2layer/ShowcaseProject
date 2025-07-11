// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/InteractionInterface.h"
#include "Logging/LogMacros.h"
#include "ShowcaseProjectCharacter.generated.h"

class UInventoryComponent;
class AShowcaseHUD;
class UItemBase;

USTRUCT()
struct FInteractionData
{
	GENERATED_USTRUCT_BODY()

	FInteractionData(): CurrentInteractable(nullptr), LastInteractionCheckTime(0.0f) {}
	
	UPROPERTY()
	AActor* CurrentInteractable;

	UPROPERTY()
	float LastInteractionCheckTime;
};

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config = Game)
class AShowcaseProjectCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	//PROPERTIES

	//Functions
	AShowcaseProjectCharacter();
	
	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent *GetCameraBoom() const { return CameraBoom; }

	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent *GetFollowCamera() const { return FollowCamera; }
	
	FORCEINLINE bool IsInteracting() const { return GetWorldTimerManager().IsTimerActive(TimerHandle_Interaction);}

	FORCEINLINE UInventoryComponent* GetInventory() const { return PlayerInventory; }

	void UpdateInteractionWidget() const;

	void AssignPrimary(UItemBase* WeaponToAssign);
	void AssignSecondary(UItemBase* WeaponToAssign);
	void EquipWeapon(UItemBase* WeaponToEquip);
	
	
protected:
	//PROPERTIES
	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent *CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent *FollowCamera;

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext *DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *LookAction;

	/** Interact Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *InteractAction;

	/** Menu and Inventory Input Actions */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *ToggleInventoryAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction *ToggleMainMenuAction;
	
	/** Target interactable object */
	UPROPERTY(VisibleAnywhere, Category="Character | Interaction")
	TScriptInterface<IInteractionInterface> TargetInteractable;

	/** Reference to the HUD */
	UPROPERTY()
	AShowcaseHUD* HUD;

	/** Frequency at which the character checks for interactable objects */
	float InteractionCheckFrequency;

	/** Distance at which the character can interact with objects */
	float InteractionCheckDistance;

	/** Timer handle for interaction checks */
	FTimerHandle TimerHandle_Interaction;

	/** Interaction data for the character */
	FInteractionData InteractionData;

	/** Inventory component */
	UPROPERTY(VisibleAnywhere, Category="Character | Inventory")
	UInventoryComponent* PlayerInventory;

	
	//Functions
	void ToggleInventoryMenu();
	void ToggleMainMenu();
	void PerformInteractionCheck();
	void FoundInteractable(AActor *NewInteractable);
	void NoInteractableFound();
	void BeginInteract();
	void EndInteract();
	void Interact();
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;


	
	/** Called for movement input */
	void Move(const FInputActionValue &Value);

	/** Called for looking input */
	void Look(const FInputActionValue &Value);

	virtual void NotifyControllerChanged() override;

	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

};
