// Copyright Epic Games, Inc. All Rights Reserved.

#include "Player/ShowcaseProjectCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "DrawDebugHelpers.h"
#include "Animation/ShowcaseAnimInstance.h"
#include "Components/InventoryComponent/InventoryComponent.h"
#include  "Components/WeaponSystemComponent/WeaponSystemComponent.h"
#include "UserInterface/ShowcaseHUD/ShowcaseHUD.h"
#include "Perception/AIPerceptionStimuliSourceComponent.h"
#include "Perception/AISense_Sight.h"
#include "UserInterface/WeaponHud/WeaponHUD.h"
#include "Weapons/WeaponBase.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AShowcaseProjectCharacter

AShowcaseProjectCharacter::AShowcaseProjectCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;			 // Character moves in the direction of input...
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;		// The camera follows at this distance behind the character
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false;								// Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character)
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)

	InteractionCheckFrequency = 0.1f; // How often to check for interactable
	InteractionCheckDistance = 200.0f; // How far away the character can interact with objects

	BaseEyeHeight = 74.0f; // The height of the character's eyes from the ground, used for interaction checks

	PlayerInventory = CreateDefaultSubobject<UInventoryComponent>(TEXT("Player Inventory"));
	PlayerInventory->SetSlotsCapacity(20);
	PlayerInventory->SetWeightCapacity(100.0f); // Set the weight capacity of the inventory

	WeaponSystemComponent = CreateDefaultSubobject<UWeaponSystemComponent>(TEXT("WeaponSystemComponent_New"));

	SetupStimuliSource();
}




//////////////////////////////////////////////////////////////////////////
// Input

void AShowcaseProjectCharacter::NotifyControllerChanged()
{
	Super::NotifyControllerChanged();

	// Add Input Mapping Context
	if (APlayerController *PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem *Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void AShowcaseProjectCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent *EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AShowcaseProjectCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AShowcaseProjectCharacter::Look);

		// Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AShowcaseProjectCharacter::BeginInteract);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Completed, this, &AShowcaseProjectCharacter::EndInteract);
		//Menu and Inventory
		EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &AShowcaseProjectCharacter::ToggleInventoryMenu);
		EnhancedInputComponent->BindAction(ToggleMainMenuAction, ETriggerEvent::Started, this, &AShowcaseProjectCharacter::ToggleMainMenu);

		//Equip Weapons
		EnhancedInputComponent->BindAction(EquipPrimary, ETriggerEvent::Started, this, &AShowcaseProjectCharacter::EquipPrimaryWeapon);
		EnhancedInputComponent->BindAction(EquipSecondary, ETriggerEvent::Started, this, &AShowcaseProjectCharacter::EquipSecondaryWeapon);
		
		// Aim, Fire and Reload
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AShowcaseProjectCharacter::BeginAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AShowcaseProjectCharacter::EndAim);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AShowcaseProjectCharacter::BeginFire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AShowcaseProjectCharacter::EndFire);

		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &AShowcaseProjectCharacter::Reload);
		
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AShowcaseProjectCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GetWorld()->TimeSince(InteractionData.LastInteractionCheckTime) > InteractionCheckFrequency)
	{
		PerformInteractionCheck();
	}

	// Handle aiming rotation
	if (bIsAiming && WeaponSystemComponent && WeaponSystemComponent->GetEquippedWeapon())
	{
		RotateTowardsCrosshair(AimingRotationSpeed, DeltaSeconds);
	}
}

void AShowcaseProjectCharacter::BeginPlay()
{
	Super::BeginPlay();
	AnimInstance = Cast<UShowcaseAnimInstance>(GetMesh()->GetAnimInstance());
	HUD = Cast<AShowcaseHUD>(GetWorld()->GetFirstPlayerController()->GetHUD());
}

void AShowcaseProjectCharacter::PerformInteractionCheck()
{
	InteractionData.LastInteractionCheckTime = GetWorld()->GetTimeSeconds();

	const FVector TraceStartLocation {GetPawnViewLocation()};

	const FVector TraceEndLocation {TraceStartLocation + (GetViewRotation().Vector() * InteractionCheckDistance)};

	float LookDirection = FVector::DotProduct(GetActorForwardVector(), GetViewRotation().Vector());

	/* Check if the character is looking in the forward direction */
	if (LookDirection > 0)
	{	
		DrawDebugLine(GetWorld(), TraceStartLocation, TraceEndLocation, FColor::Green, false, 1.0f, 0, 1.0f);
		
		FCollisionQueryParams QueryParams;

		QueryParams.AddIgnoredActor(this); // Ignore self in trace

		FHitResult TraceHitResult;
	
		if (GetWorld()->LineTraceSingleByChannel(TraceHitResult, TraceStartLocation, TraceEndLocation, ECC_Visibility, QueryParams))
		{
			if (TraceHitResult.GetActor()->GetClass()->ImplementsInterface(UInteractionInterface::StaticClass()))
			{
				// Check if the interactable is within the interaction distance and is not the current interactable Call FoundInteractable
				if (TraceHitResult.GetActor() != InteractionData.CurrentInteractable )
				{
					FoundInteractable(TraceHitResult.GetActor());
					return;
				}
				// If the interactable is the current interactable, we can skip further checks and return
				if (TraceHitResult.GetActor() == InteractionData.CurrentInteractable)
				{
					return;
				}
			}
		}
	}
	// If we reach this point, no interactable was found
	NoInteractableFound();
}

void AShowcaseProjectCharacter::FoundInteractable(AActor* NewInteractable)
{
	if (IsInteracting())
	{
		EndInteract();
	}
	
	if (InteractionData.CurrentInteractable)
	{
		TargetInteractable = InteractionData.CurrentInteractable;
		TargetInteractable->EndFocus();
	}
	// Set the new interactable
	InteractionData.CurrentInteractable = NewInteractable;
	TargetInteractable = NewInteractable;

	HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	
	TargetInteractable->BeginFocus();
}

void AShowcaseProjectCharacter::NoInteractableFound()
{
	if (IsInteracting())
	{
		GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);
	}

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
			TargetInteractable->EndFocus();
		}

		// Hide the Interaction Widget on the HUD
		HUD->HideInteractionWidget();
		
		InteractionData.CurrentInteractable = nullptr;
		TargetInteractable = nullptr;
	}
}

void AShowcaseProjectCharacter::BeginInteract()
{
	//Verify nothing has changed with the interactable state since the beginning of the interaction check
	PerformInteractionCheck();

	if (InteractionData.CurrentInteractable)
	{
		if (IsValid(TargetInteractable.GetObject()))
		{
		TargetInteractable->BeginInteract();

			if (FMath::IsNearlyZero(TargetInteractable->InteractableData.InteractionDuration, 0.1f))
			{
			Interact();
			}
			else
			{
				GetWorldTimerManager().SetTimer(TimerHandle_Interaction, this, &AShowcaseProjectCharacter::Interact, TargetInteractable->InteractableData.InteractionDuration, false);
			}
		}
	}
	
}

void AShowcaseProjectCharacter::EndInteract()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->EndInteract();
	}
}

void AShowcaseProjectCharacter::Interact()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_Interaction);

	if (IsValid(TargetInteractable.GetObject()))
	{
		TargetInteractable->Interact(this);
	}
}

void AShowcaseProjectCharacter::BeginAim()
{
	if (WeaponSystemComponent && WeaponSystemComponent->GetEquippedWeapon())
	{
		bIsAiming = true;

		//Slow down movement when aiming
		GetCharacterMovement()->MaxWalkSpeed = AimWalkSpeed;

		//Zoom in camera when aiming (adjust FOX)
		if (FollowCamera)
		{
			if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
			{
				PlayerController->SetViewTargetWithBlend(this, 0.2f, VTBlend_Linear, 0.0f, false);
				PlayerController->PlayerCameraManager->SetFOV(AimFOV);
			}
		}
		
		// Show weapon HUD when aiming
		if (HUD && WeaponSystemComponent->GetEquippedWeapon())
		{
			HUD->GetWeaponHUD()->StartAiming();
		}
		UE_LOG(LogTemplateCharacter, Log, TEXT("Started aiming"));
	}
}

void AShowcaseProjectCharacter::EndAim()
{
	if (WeaponSystemComponent && WeaponSystemComponent->GetEquippedWeapon())
	{
		bIsAiming = false;

		//Restore normal movement speed
		GetCharacterMovement()->MaxWalkSpeed = NormalWalkSpeed;
		//Restore normal camera FOV
		if (FollowCamera)
		{
			if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
			{
				PlayerController->SetViewTargetWithBlend(this, 0.2f, VTBlend_Linear, 0.0f, false);
				PlayerController->PlayerCameraManager->SetFOV(NormalFOV);
			}
		}
		// Hide weapon HUD when stopping aim
		if (HUD && WeaponSystemComponent->GetEquippedWeapon())
		{
			HUD->GetWeaponHUD()->StopAiming();
			HUD->GetWeaponHUD()->HideCrosshair();
		}
		
		UE_LOG(LogTemplateCharacter, Log, TEXT("Stopped aiming"));
	}
}

void AShowcaseProjectCharacter::UpdateCrosshairDirection()
{
	if (const APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		// Get viewport size
		FVector2D ViewportSize;
		GetWorld()->GetGameViewport()->GetViewportSize(ViewportSize);
		const FVector2D CrosshairScreenPosition = ViewportSize * 0.5f;

		// Convert crosshair screen position to world space
		FVector CrosshairWorldLocation;
		PC->DeprojectScreenPositionToWorld(
			CrosshairScreenPosition.X,
			CrosshairScreenPosition.Y,
			CrosshairWorldLocation,
			CrosshairWorldDirection
		);
	}
}

void AShowcaseProjectCharacter::RotateTowardsCrosshair(float RotationSpeed, float DeltaTime)
{
	UpdateCrosshairDirection();

	// Calculate target location in world space
	const FVector CharacterLocation = GetActorLocation();
	const FVector TargetLocation = CharacterLocation + (CrosshairWorldDirection * 1000.0f);

	// Calculate direction from character to target (only use X and Y for yaw rotation)
	FVector DirectionToTarget = (TargetLocation - CharacterLocation).GetSafeNormal();
	DirectionToTarget.Z = 0.0f; // Remove vertical component for character rotation

	if (!DirectionToTarget.IsZero())
	{
		// Calculate target rotation
		FRotator TargetRotation = DirectionToTarget.Rotation();

		// Get current rotation
		FRotator CurrentRotation = GetActorRotation();

		// Interpolate to target rotation
		const FRotator NewRotation = FMath::RInterpTo(
			CurrentRotation,
			TargetRotation,
			DeltaTime,
			RotationSpeed
		);

		// Apply the rotation
		SetActorRotation(NewRotation);
	}
}

void AShowcaseProjectCharacter::SetupStimuliSource()
{
	StimuliSource = CreateDefaultSubobject<UAIPerceptionStimuliSourceComponent>(TEXT("Stimulus"));
	if (StimuliSource)
	{
		StimuliSource->RegisterForSense(TSubclassOf<UAISense_Sight>());
		StimuliSource->RegisterWithPerceptionSystem();
	}
}

void AShowcaseProjectCharacter::BeginFire()
{
	if (WeaponSystemComponent)
	{
		// Rotate quickly towards crosshair when firing (if not already aiming)
		if (!bIsAiming)
		{
			RotateTowardsCrosshair(FiringRotationSpeed, GetWorld()->GetDeltaSeconds());
		}
		
		WeaponSystemComponent->StartFire();
		if (HUD && WeaponSystemComponent->GetEquippedWeapon())
		{
			HUD->OnWeaponFired();
		}
		else
		{
			UE_LOG(LogTemplateCharacter, Error, TEXT("HUD is not initialized!"));
		}
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("WeaponSystemComponent is not initialized!"));
	}
}

void AShowcaseProjectCharacter::EndFire()
{
	if (WeaponSystemComponent)
	{
			WeaponSystemComponent->StopFire();
		if (HUD && WeaponSystemComponent->GetEquippedWeapon())
		{
			HUD->OnWeaponStoppedFiring();
		}
		else
		{
			UE_LOG(LogTemplateCharacter, Error, TEXT("HUD is not initialized!"));
		}
	}
	else
	{
			UE_LOG(LogTemplateCharacter, Error, TEXT("WeaponSystemComponent is not initialized!"));
	}
}

void AShowcaseProjectCharacter::EquipPrimaryWeapon()
{
	if(!WeaponSystemComponent) return;

	if (!WeaponSystemComponent->GetWeaponInSlot(EWeaponSlot::Primary))
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("No weapon assigned to Primary slot!"));
		return;
	}

	AWeaponBase* PrimaryWeapon = WeaponSystemComponent->GetWeaponInSlot(EWeaponSlot::Primary);
	if (WeaponSystemComponent->GetEquippedWeapon() == PrimaryWeapon)
	{
		// If primary weapon is already equipped, holster it
		WeaponSystemComponent->HolsterWeaponWithAnimation(EWeaponSlot::Primary);
		return;
	}

	// Draw the weapon (the component will handle holstering current weapon if needed)
	WeaponSystemComponent->DrawWeaponWithAnimation(EWeaponSlot::Primary);
}

void AShowcaseProjectCharacter::EquipSecondaryWeapon()
{
	if(!WeaponSystemComponent) return;

	if (!WeaponSystemComponent->GetWeaponInSlot(EWeaponSlot::Secondary))
	{
		UE_LOG(LogTemplateCharacter, Warning, TEXT("No weapon assigned to Secondary slot!"));
		return;
	}

	AWeaponBase* SecondaryWeapon = WeaponSystemComponent->GetWeaponInSlot(EWeaponSlot::Secondary);
	if (WeaponSystemComponent->GetEquippedWeapon() == SecondaryWeapon)
	{
		// If secondary weapon is already equipped, holster it
		WeaponSystemComponent->HolsterWeaponWithAnimation(EWeaponSlot::Secondary);
		return;
	}

	// Draw the weapon (the component will handle holstering current weapon if needed)
	WeaponSystemComponent->DrawWeaponWithAnimation(EWeaponSlot::Secondary);
}

void AShowcaseProjectCharacter::Reload()
{
	if (WeaponSystemComponent)
	{
		UE_LOG(LogTemplateCharacter, Log, TEXT("Reloading weapon"));
		WeaponSystemComponent->Reload();
	}
}

void AShowcaseProjectCharacter::UpdateInteractionWidget() const
{
	if (IsValid(TargetInteractable.GetObject()))
	{
		HUD->UpdateInteractionWidget(&TargetInteractable->InteractableData);
	}
}

void AShowcaseProjectCharacter::ToggleInventoryMenu() 
{
	HUD->ToggleInventoryMenu();
}

void AShowcaseProjectCharacter::ToggleMainMenu() 
{
	UE_LOG(LogTemp, Warning, TEXT("Toggling Main Menu"));
	HUD->ToggleMainMenu();
}

void AShowcaseProjectCharacter::Move(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AShowcaseProjectCharacter::Look(const FInputActionValue &Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}