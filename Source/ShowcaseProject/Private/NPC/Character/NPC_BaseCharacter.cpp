// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Character/NPC_BaseCharacter.h"

#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "NPC/Controller/NPC_AIController.h"
#include "Player/ShowcaseProjectCharacter.h"
#include "Components/DialogueComponent/DialogueComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
ANPC_BaseCharacter::ANPC_BaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
	StateTreeComponent = CreateDefaultSubobject<UShowcaseStateTreeComponent>(TEXT("StateTreeComponent"));
	MaxHealth = 100.0f;
	CurrentHealth = MaxHealth;
	bIsInvulnerable = false;
	DamageResistance = 0.0f;
	bIsDead = false;
	DeathDelayTime = 5.0f;
	RagdollImpulseStrength = 500.0f;
	bCanRagdoll = true;

	// Set default bone damage multipliers if not already set
	if (BoneDamageMultipliers.Num() == 0)
	{
		BoneDamageMultipliers.Add(TEXT("head"), 2.0f);
		BoneDamageMultipliers.Add(TEXT("Head"), 2.0f);
		BoneDamageMultipliers.Add(TEXT("skull"), 2.0f);
		BoneDamageMultipliers.Add(TEXT("spine_03"), 1.5f);
		BoneDamageMultipliers.Add(TEXT("spine_02"), 1.2f);
		BoneDamageMultipliers.Add(TEXT("spine_01"), 1.0f);
	}
    
	// Initialize internal state
	CurrentAlertness = 0.0f;
	TimeInCurrentState = 0.0f;
	CurrentTarget = nullptr;
	LastKnownPlayerLocation = nullptr;
}

// Called when the game starts or when spawned
void ANPC_BaseCharacter::BeginPlay()
{
	Super::BeginPlay();
	InteractableData.InteractableType = EInteractableType::NonPlayerCharacter;
	InteractableData.Name = FText::FromString(GetName());
	InteractableData.Action = FText::FromString("Talk");
	InteractableData.Quantity = 0;
	InteractableData.InteractionDuration = 0.0f;

	//Log the StateTree
	UE_LOG(LogTemp, Log, TEXT("NPC %s StateTree: %s"), *GetName(), *StateTreeComponent->GetName());
    
	// Initialize from data table if needed
	InitializeFromDataTable();

	// Set up AI controller if available
	if (USkeletalMeshComponent* MeshComp = GetMesh())
	{
		MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		MeshComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block);
		MeshComp->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Block);
		MeshComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Block);
		MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Overlap);
        
		// Enable complex collision for accurate bone hit detection
		MeshComp->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
		MeshComp->SetCanEverAffectNavigation(false);
	}

	
	// Set initial health
	if (MaxHealth <= 0.0f)
	{
		MaxHealth = 100.0f;
	}
	CurrentHealth = MaxHealth;
    
	// Set initial state
	SetNPCState(ENPCState::Idle);
}

void ANPC_BaseCharacter::BeginDestroy()
{
	Super::BeginDestroy();
}

void ANPC_BaseCharacter::OnStateChanged(ENPCState OldState, ENPCState NewState)
{
	return;
}

// Called every frame
void ANPC_BaseCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ANPC_BaseCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANPC_BaseCharacter::InitializeFromDataTable()
{
	if (NPCDataHandle.DataTable && NPCDataHandle.RowName != NAME_None)
	{
		FST_NPCDataStruct* RowData = NPCDataHandle.DataTable->FindRow<FST_NPCDataStruct>(NPCDataHandle.RowName, TEXT(""));
		if (RowData)
		{
			NPCData = *RowData;

			//Apply data from the table
			MaxHealth = NPCData.MaxHealth;
			CurrentHealth = MaxHealth;

			//Update movement speeds
			if (GetCharacterMovement())
			{
				GetCharacterMovement()->MaxWalkSpeed = 500;
			}
			UE_LOG(LogTemp, Log, TEXT("NPC %s initialized from data table"), *GetName());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Failed to find row %s in data table"), *NPCDataHandle.RowName.ToString());
		}
	}
}

void ANPC_BaseCharacter::SetNPCState(ENPCState NewState)
{
	if (CurrentState != NewState)
	{
		ENPCState OldState = CurrentState;
		PreviousState = CurrentState;
		CurrentState = NewState;
		TimeInCurrentState = 0.0f;

		OnStateChanged(OldState, NewState);

		//Update blackboard if we have an AI controller
		if (ANPC_AIController* AIController = Cast<ANPC_AIController>(GetController()))
		{
			if (UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent())
			{
				// BlackboardComponent->SetValueAsEnum(ANPC_AIController::BB_CurrentState, static_cast<uint8>(NewState));
			}
		}
	}
}

bool ANPC_BaseCharacter::CanStartDialogue(AActor* Initiator)
{
	if (!DialogueComponent) return false;
	
	return DialogueComponent->CanStartDialogue(Initiator);
}

void ANPC_BaseCharacter::StartDialogue(AActor* DialoguePartner)
{
	if (DialogueComponent)
	{
		DialogueComponent->StartDialogue(DialoguePartner, NPCData.InitialDialogueNode);
	}
}

void ANPC_BaseCharacter::EndDialogue()
{
	if (DialogueComponent)
	{
		DialogueComponent->EndDialogue();
	}
}

// InteractionInterface functions

void ANPC_BaseCharacter::BeginFocus()
{
	UE_LOG(LogTemp, Log, TEXT("NPC %s is focused"), *GetName());
	if (GetMesh())
	{
		GetMesh()->SetRenderCustomDepth(true);
	}
}

void ANPC_BaseCharacter::EndFocus()
{
	if (GetMesh())
	{
		GetMesh()->SetRenderCustomDepth(false);
	}
}

void ANPC_BaseCharacter::BeginInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Begin Interact Override on interface test actor"));
}

void ANPC_BaseCharacter::EndInteract()
{
	UE_LOG(LogTemp, Warning, TEXT("Calling End Interact Override on interface test actor"));
}

void ANPC_BaseCharacter::Interact(AShowcaseProjectCharacter* PlayerCharacter)
{
	UE_LOG(LogTemp, Warning, TEXT("Calling Interact Override on interface test actor"));

	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Interact called with null PlayerCharacter"));
		return;
	}
	//Check if we can start dialogue
	if (CanStartDialogue(PlayerCharacter))
	{
		UE_LOG(LogTemp, Log, TEXT("Starting dialogue with %s"), *GetName());
		StartDialogue(PlayerCharacter);
        
		// Set InteractableData for the interface
		InteractableData.InteractableType = EInteractableType::NonPlayerCharacter;
		InteractableData.Name = FText::FromString(GetName());
		InteractableData.Action = FText::FromString("Talk");
		InteractableData.Quantity = 0;
		InteractableData.InteractionDuration = 0.0f;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Cannot start dialogue with %s"), *GetName());
	}
}

// DamageInterface functions

float ANPC_BaseCharacter::TakeDamage(float DamageAmount, const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if (!CanTakeDamage(DamageEvent, EventInstigator, DamageCauser) || bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("%s cannot take damage: Dead=%d, Invulnerable=%d"), 
			*GetName(), bIsDead, bIsInvulnerable);
		return 0.0f; // Cannot take damage if dead or not allowed
	}

	//Get bone name from projectile damage event
	FName BoneName = NAME_None;
	if (const FProjectileDamageEvent* ProjectileEvent = static_cast<const FProjectileDamageEvent*>(&DamageEvent))
	{
		BoneName = ProjectileEvent->BoneName;
	}

	//Apply damage multiplier and modificatoins
	float ModifiedDamage = ModifyIncomingDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	ModifiedDamage *= GetDamageMultiplier(DamageEvent, BoneName);
	UE_LOG(LogTemp, Log, TEXT("%s received %.2f damage (base: %.2f, modified: %.2f) from %s"), 
		*GetName(), DamageAmount, ModifiedDamage / GetDamageMultiplier(DamageEvent, BoneName), ModifiedDamage, *DamageCauser->GetName());
	
	//Apply damage
	float OldHealth = CurrentHealth;

	CurrentHealth = FMath::Max(0.0f, CurrentHealth - ModifiedDamage);

	UE_LOG(LogTemp, Log, TEXT("%s took %.2f damage from %s. Health: %.2f/%.2f"), 
		*GetName(), ModifiedDamage, *DamageCauser->GetName(), CurrentHealth, MaxHealth);

	//Process damage effects
	ProcessDamageEffects(ModifiedDamage, DamageEvent, DamageCauser);

	UE_LOG(LogTemp, Log, TEXT("%s health after damage: %.2f"), *GetName(), CurrentHealth);
	
	//Trigger events
	OnDamageTakenDelegate.Broadcast(ModifiedDamage, DamageCauser, DamageEvent);

	UE_LOG(LogTemp, Log, TEXT("%s health changed from %.2f to %.2f"), *GetName(), OldHealth, CurrentHealth);
	
	// OnHealthChanged(CurrentHealth, OldHealth);

	//Check for death

	if (CurrentHealth <= 0.0f && !bIsDead)
	{
		Die(DamageEvent, EventInstigator, DamageCauser);
	}

	// Set combat state if not already dead
	if (!bIsDead && CurrentState != ENPCState::Combat)
	{
		SetNPCState(ENPCState::Combat);
		if (DamageCauser && DamageCauser->IsA<APawn>())
		{
			CurrentTarget = DamageCauser;
		}
	}

	return ModifiedDamage;
}

void ANPC_BaseCharacter::Die(const FDamageEvent& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (bIsDead) return;

	bIsDead = true;
	CurrentHealth = 0.0f;
	SetNPCState(ENPCState::Dead);

	UE_LOG(LogTemp, Log, TEXT("%s has died"), *GetName());

	//Get Impact information for ragdoll
	FVector ImpulseLocation = GetActorLocation();
	FVector ImpulseDirection = GetActorForwardVector();

	if (const FProjectileDamageEvent* ProjectileEvent = static_cast<const FProjectileDamageEvent*>(&DamageEvent))
	{
		ImpulseLocation = ProjectileEvent->HitLocation;
		ImpulseDirection = ProjectileEvent->HitDirection;
	}

	//Trigger death events
	OnDeathDelegate.Broadcast(DamageCauser, DamageEvent);
	//Enable ragdoll physics
	if (ShouldRagdollOnDeath() && bCanRagdoll)
	{
		EnableRagdoll(ImpulseLocation, ImpulseDirection);
	}

	//Set cleanup timer
	GetWorldTimerManager().SetTimer(
		DeathCleanupTimer,
		this,
		&ANPC_BaseCharacter::CleanupAfterDeath,
		DeathDelayTime,
		false
		);
}


void ANPC_BaseCharacter::EnableRagdoll(const FVector& ImpulseLocation, const FVector& ImpulseDirection)
{
	if (!GetMesh() || bIsDead == false) return;

	//Disable colission with capsule component
    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Enable physics simulation on mesh
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	GetMesh()->SetSimulatePhysics(true);

	//Apply death impulse
	ApplyDeathImpulse(ImpulseLocation, ImpulseDirection);

	UE_LOG(LogTemp, Log, TEXT("%s enabled ragdoll physics"), *GetName());
}

void ANPC_BaseCharacter::ApplyDeathImpulse(const FVector& ImpulseLocation, const FVector& ImpulseDirection)
{
	if (!GetMesh() || ImpulseDirection.IsZero()) return;

	//Calculate impulse force
	FVector ImpulseForce = ImpulseDirection.GetSafeNormal() * RagdollImpulseStrength;

	//Find closest bone to impact location

	FName ClosestBone = GetMesh()->FindClosestBone(ImpulseLocation);
	if (ClosestBone != NAME_None)
	{
		//Apply impulse to the closest bone
		GetMesh()->AddImpulseAtLocation(ImpulseForce, ImpulseLocation, ClosestBone);
		UE_LOG(LogTemp, Log, TEXT("Applied ragdoll impulse to bone: %s"), *ClosestBone.ToString());
	}
	else
	{
		//Fallback to root bone if no closest bone found
		GetMesh()->AddImpulseAtLocation(ImpulseForce, ImpulseLocation, GetMesh()->GetBoneName(0));
		UE_LOG(LogTemp, Log, TEXT("%s applied death impulse to root bone"), *GetName());
	}
}

bool ANPC_BaseCharacter::CanTakeDamage(const FDamageEvent& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser) const
{
	if (bIsDead)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPC %s is already dead"), *GetName());
		return false;
	}
    
	if (bIsInvulnerable)
	{
		UE_LOG(LogTemp, Warning, TEXT("NPC %s is invulnerable"), *GetName());
		return false;
	}
    
	// Don't take damage from self
	if (DamageCauser == this)
	{
		return false;
	}
    
	return true;
}

float ANPC_BaseCharacter::ModifyIncomingDamage(float BaseDamage, const FDamageEvent& DamageEvent,
	AController* EventInstigator, AActor* DamageCauser) const
{
	return IDamageableInterface::ModifyIncomingDamage(BaseDamage, DamageEvent, EventInstigator, DamageCauser);
}

void ANPC_BaseCharacter::HandleDeath()
{
	// Disable AI
	if (ANPC_AIController* AIController = Cast<ANPC_AIController>(GetController()))
	{
		AIController->StopMovement();
		AIController->GetBrainComponent()->StopLogic(TEXT("NPC is dead"));
	}
    
	// Disable collision for gameplay but keep physics for ragdoll
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    
	// Stop movement
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
}

void ANPC_BaseCharacter::CleanupAfterDeath()
{
	UE_LOG(LogTemp, Log, TEXT("Cleaning up dead NPC: %s"), *GetName());
    
	// Add any cleanup logic here (drop items, etc.)
	// For now, just destroy the actor
	Destroy();
}

float ANPC_BaseCharacter::GetDamageMultiplier(const FDamageEvent& DamageEvent, const FName& BoneName) const
{
	if (BoneName != NAME_None && BoneDamageMultipliers.Contains(BoneName))
	{
		return BoneDamageMultipliers[BoneName];
	}

	// Check for partial bone name matches
	for (const auto& BoneMultiplier : BoneDamageMultipliers)
	{
		if (BoneName.ToString().Contains(BoneMultiplier.Key.ToString()))
		{
			return BoneMultiplier.Value;
		}
	}

	return 1.0f; // Default multiplier
}


void ANPC_BaseCharacter::ProcessDamageEffects(float DamageAmount, const FDamageEvent& DamageEvent, AActor* DamageCauser)
{
	// Play damage sound effects, spawn blood particles, etc.
    
	// Example: Spawn blood effect at hit location
	if (const FProjectileDamageEvent* ProjectileEvent = static_cast<const FProjectileDamageEvent*>(&DamageEvent))
	{
		// Spawn blood/impact effect at ProjectileEvent->HitLocation
		UE_LOG(LogTemp, Log, TEXT("Blood effect should spawn at: %s"), *ProjectileEvent->HitLocation.ToString());
	}

}
