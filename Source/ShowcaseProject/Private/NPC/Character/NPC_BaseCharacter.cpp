// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Character/NPC_BaseCharacter.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NPC/Controller/NPC_AIController.h"
#include "Player/ShowcaseProjectCharacter.h"
#include "Components/DialogueComponent/DialogueComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/AIPerceptionTypes.h"

// Sets default values
ANPC_BaseCharacter::ANPC_BaseCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DialogueComponent = CreateDefaultSubobject<UDialogueComponent>(TEXT("DialogueComponent"));
	NPCAIController = Cast<ANPC_AIController>(GetController());

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
    
	// Initialize from data table if needed
	InitializeFromDataTable();
    
	// Set initial health
	if (MaxHealth <= 0.0f)
	{
		MaxHealth = 100.0f;
	}
	CurrentHealth = MaxHealth;
    
	// Set initial state
	SetNPCState(ENPCState::Idle);
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
				BlackboardComponent->SetValueAsEnum(ANPC_AIController::BB_CurrentState, static_cast<uint8>(NewState));
			}
		}
	}
}

ENPCRelationship ANPC_BaseCharacter::GetRelationshipWithActor(AActor* OtherActor)
{
	if (!OtherActor) return ENPCRelationship::Neutral;

	//Check if it's a player character
	if (OtherActor->IsA<AShowcaseProjectCharacter>())
	{
		FGameplayTag PlayerFactionTag = FGameplayTag::RequestGameplayTag("Faction.Player");
		float RelationValue = GetFactionRelationValue(PlayerFactionTag);

		if (RelationValue >= 0.75f) return ENPCRelationship::Ally;
		if (RelationValue >= 0.25f) return ENPCRelationship::Friendly;
		if (RelationValue >= -0.25f) return ENPCRelationship::Neutral;
		return ENPCRelationship::Enemy;
	}
	//For other NPCs, check their faction
	if (ANPC_BaseCharacter* OtherNPC = Cast<ANPC_BaseCharacter>(OtherActor))
	{
		FGameplayTag OtherFactionTag = OtherNPC->NPCData.FactionTag;
		float RelationValue = GetFactionRelationValue(OtherFactionTag);

		if (RelationValue >= 0.75f) return ENPCRelationship::Ally;
		if (RelationValue >= 0.25f) return ENPCRelationship::Friendly;
		if (RelationValue >= -0.25f) return ENPCRelationship::Neutral;
		return ENPCRelationship::Enemy;
	}
	return ENPCRelationship::Neutral; // Default case
}

float ANPC_BaseCharacter::GetFactionRelationValue(FGameplayTag FactionTag)
{
	if (NPCData.FactionRelations.Contains(FactionTag))
	{
		return NPCData.FactionRelations[FactionTag];
	}
	
	return 0.0f; // neutral relation if not found
}

void ANPC_BaseCharacter::SetFactionRelation(FGameplayTag FactionTag, float RelationValue)
{
	NPCData.FactionRelations.Add(FactionTag, FMath::Clamp(RelationValue, -1.0f, 1.0f));
}

bool ANPC_BaseCharacter::ShouldFlee()
{
	float HealthPercentage = CurrentHealth / MaxHealth;
	float FleeThreshold = NPCData.FleeHealthThreshold;

	return HealthPercentage <= FleeThreshold && CurrentState != ENPCState::Dead && CurrentState != ENPCState::Combat;
}

bool ANPC_BaseCharacter::ShouldTakeCover()
{
	return (CurrentHealth / MaxHealth) <= NPCData.TakeCoverHealthThreshold && 
		CurrentState != ENPCState::Dead && 
		CurrentState != ENPCState::Combat;
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

void ANPC_BaseCharacter::OnStateChanged(ENPCState OldState, ENPCState NewState)
{
	UE_LOG(LogTemp, Log, TEXT("%s: State changed from %d to %d"), *GetName(), static_cast<int32>(OldState), static_cast<int32>(NewState));

	//Handle state-specific logic
	switch (NewState) {
	case ENPCState::Idle:
	case ENPCState::Patrol:
		GetCharacterMovement()->MaxWalkSpeed = 150.0f; // Normal walking speed
		break;
	case ENPCState::Alert:
		GetCharacterMovement()->MaxWalkSpeed = 200.0f; // Increased speed when alert
		break;
	case ENPCState::Combat:
	case ENPCState::Flee:
		GetCharacterMovement()->MaxWalkSpeed = 600.0f; // Fast speed during combat or fleeing
	case ENPCState::TakeCover:
		break;
	case ENPCState::Dialogue:
		GetCharacterMovement()->MaxWalkSpeed = 0.0f; // Stop movement during dialogue
		break;
	case ENPCState::Dead:
		break;
	}
	
}

float ANPC_BaseCharacter::CalculateReactionIntensity(FGameplayTag ReactionType)
{
	// Base reaction intensity calculation based on personality
	float BaseIntensity = 0.5f;

	if (ReactionType.MatchesTag(FGameplayTag::RequestGameplayTag("Reaction.Fear")))
	{
		BaseIntensity = 1.0f - NPCData.Courage; // Higher courage means lower fear reaction
	}
	else if (ReactionType.MatchesTag(FGameplayTag::RequestGameplayTag("Reaction.Aggression")))
	{
		BaseIntensity = NPCData.Aggressiveness; // Higher aggressiveness means stronger aggression reaction
	}
	else if (ReactionType.MatchesTag(FGameplayTag::RequestGameplayTag("Reaction.Calm")))
	{
		BaseIntensity = 1.0f - NPCData.Alertness; // Higher alertness means lower calm reaction
	}

	return FMath::Clamp(BaseIntensity, 0.0f, 1.0f);
}

void ANPC_BaseCharacter::ReactToStimulus(FGameplayTag StimulusType, AActor* Source, float Intensity)
{
	if (!Source || CurrentState == ENPCState::Dead)
	{
		return;
	}
    
	float ReactionIntensity = CalculateReactionIntensity(StimulusType) * Intensity;
    
	if (StimulusType.MatchesTag(FGameplayTag::RequestGameplayTag("Stimulus.WeaponDrawn")))
	{
		if (ReactionIntensity > 0.7f)
		{
			SetNPCState(ENPCState::Alert);
			CurrentTarget = Source;
		}
	}
	else if (StimulusType.MatchesTag(FGameplayTag::RequestGameplayTag("Stimulus.Damage")))
	{
		SetNPCState(ENPCState::Combat);
		CurrentTarget = Source;
	}
	else if (StimulusType.MatchesTag(FGameplayTag::RequestGameplayTag("Stimulus.PlayerSeen")))
	{
		ENPCRelationship Relationship = GetRelationshipWithActor(Source);
        
		if (Relationship == ENPCRelationship::Enemy && ReactionIntensity > 0.5f)
		{
			SetNPCState(ENPCState::Combat);
			CurrentTarget = Source;
		}
		else if (Relationship == ENPCRelationship::Neutral || Relationship == ENPCRelationship::Friendly)
		{
			SetNPCState(ENPCState::Alert);
		}
	}
}

void ANPC_BaseCharacter::OnWeaponDrawn(AActor* WeaponOwner)
{
	ReactToStimulus(FGameplayTag::RequestGameplayTag("Stimulus.WeaponDrawn"), WeaponOwner, 1.0f);
}

void ANPC_BaseCharacter::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	for (AActor* Actor : UpdatedActors)
	{
		if (AShowcaseProjectCharacter* Player = Cast<AShowcaseProjectCharacter>(Actor))
		{
			ENPCRelationship Relationship = GetRelationshipWithActor(Player);
            
			if (Relationship == ENPCRelationship::Enemy)
			{
				SetNPCState(ENPCState::Combat);
				CurrentTarget = Player;
			}
			else if (CurrentState == ENPCState::Idle || CurrentState == ENPCState::Patrol)
			{
				SetNPCState(ENPCState::Alert);
			}
		}
	}
}

void ANPC_BaseCharacter::OnTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus)
{
	if (AShowcaseProjectCharacter* Player = Cast<AShowcaseProjectCharacter>(Actor))
	{
		if (Stimulus.WasSuccessfullySensed())
		{
			// Player spotted
			ReactToStimulus(FGameplayTag::RequestGameplayTag("Stimulus.PlayerSeen"), Player, 1.0f);
            
			// Update blackboard
			if (ANPC_AIController* AIController = Cast<ANPC_AIController>(GetController()))
			{
				if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
				{
					BlackboardComp->SetValueAsVector(ANPC_AIController::BB_PlayerLocation, Player->GetActorLocation());
					BlackboardComp->SetValueAsVector(ANPC_AIController::BB_LastKnownPlayerLocation, Player->GetActorLocation());
				}
			}
		}
		else
		{
			// Player lost
			if (CurrentTarget == Player && (CurrentState == ENPCState::Alert || CurrentState == ENPCState::Combat))
			{
				SetNPCState(ENPCState::Patrol); // Return to patrol or investigate
			}
		}
	}
}

void ANPC_BaseCharacter::OnHealthChanged(float NewHealth, float OldHealth)
{
	UE_LOG(LogTemp, Log, TEXT("%s: Health changed from %.2f to %.2f"), *GetName(), OldHealth, NewHealth);
    
	// Update AI controller with health status
	if (ANPC_AIController* AIController = Cast<ANPC_AIController>(GetController()))
	{
		if (UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent())
		{
			BlackboardComp->SetValueAsBool(ANPC_AIController::BB_ShouldFlee, ShouldFlee());
			BlackboardComp->SetValueAsBool(ANPC_AIController::BB_ShouldTakeCover, ShouldTakeCover());
		}
	}
}

void ANPC_BaseCharacter::OnDeath()
{
	SetNPCState(ENPCState::Dead);
    
	// Stop all movement
	GetCharacterMovement()->MaxWalkSpeed = 0.0f;
	GetCharacterMovement()->StopMovementImmediately();
    
	// End dialogue if active
	if (bIsInDialogue)
	{
		EndDialogue();
	}
    
	UE_LOG(LogTemp, Log, TEXT("%s has died"), *GetName());
}

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

void ANPC_BaseCharacter::TakeDamage(float DamageAmount, AActor* DamageCauser)
{
	if (CurrentState == ENPCState::Dead)
	{
		return;
	}
    
	float OldHealth = CurrentHealth;
	CurrentHealth = FMath::Max(0.0f, CurrentHealth - DamageAmount);
    
	OnHealthChanged(CurrentHealth, OldHealth);
    
	// React to damage
	ReactToStimulus(FGameplayTag::RequestGameplayTag("Stimulus.Damage"), DamageCauser, DamageAmount / MaxHealth);
    
	// Check if should flee or take cover
	if (ShouldFlee())
	{
		SetNPCState(ENPCState::Flee);
	}
	else if (ShouldTakeCover())
	{
		SetNPCState(ENPCState::TakeCover);
	}
    
	// Check for death
	if (CurrentHealth <= 0.0f)
	{
		OnDeath();
	}

}

