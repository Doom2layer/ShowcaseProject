// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/DamageableInterface.h"
#include "Data/FST_NPCDataStruct.h"
#include "Interfaces/InteractionInterface.h"
#include "NPC_BaseCharacter.generated.h"

class ANPC_AIController;
class UDialogueComponent;
class APatrolPath;
class UBehaviorTree;
class UInputComponent;
struct FST_NPCDataStruct;
class UAnimMontage;
class AActor;

UENUM(BlueprintType)
enum class ENPCState : uint8
{
	Idle,
	Patrol,
	Alert,
	Combat,
	Flee,
	TakeCover,
	Dialogue,
	Dead
};
UENUM(BlueprintType)
enum class ENPCRelationship : uint8
{
	Enemy,
	Neutral,
	Friendly,
	Ally
};

UCLASS()
class SHOWCASEPROJECT_API ANPC_BaseCharacter : public ACharacter, public IDamageableInterface, public IInteractionInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC_BaseCharacter();

	// Initialization
	UFUNCTION(BlueprintCallable, Category = "NPC")
	void InitializeFromDataTable();

	// State Management
	UFUNCTION(BlueprintCallable, Category = "State")
	void SetNPCState(ENPCState NewState);

	UFUNCTION(BlueprintPure, Category = "State")
	ENPCState GetCurrentState() const { return CurrentState; }

	// Relationship System
	UFUNCTION(BlueprintPure, Category = "Faction")
	ENPCRelationship GetRelationshipWithActor(AActor* OtherActor);

	UFUNCTION(BlueprintPure, Category = "Faction")
	float GetFactionRelationValue(FGameplayTag FactionTag);

	UFUNCTION(BlueprintCallable, Category = "Faction")
	void SetFactionRelation(FGameplayTag FactionTag, float RelationValue);

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool ShouldFlee();

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool ShouldTakeCover();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OnWeaponDrawn(AActor* WeaponOwner);

	// Dialogue System
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool CanStartDialogue(AActor* Initiator);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void StartDialogue(AActor* DialoguePartner);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();

	// Personality-based Reactions
	UFUNCTION(BlueprintPure, Category = "Personality")
	float CalculateReactionIntensity(FGameplayTag ReactionType);

	UFUNCTION(BlueprintCallable, Category = "Personality")
	void ReactToStimulus(FGameplayTag StimulusType, AActor* Source, float Intensity);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	FORCEINLINE APatrolPath* GetPatrolPath() const { return PatrolPath; }

	FORCEINLINE UAnimMontage* GetHookMontage() const { return HookMontage; }

	// Damageable Interface
	virtual void TakeDamage(float DamageAmount, AActor* DamageCauser) override;

	// Data Table Row Reference
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "NPC Data")
	FDataTableRowHandle NPCDataHandle;

	// Current NPC Data (cached from data table)
	UPROPERTY(BlueprintReadOnly, Category = "NPC Data")
	FST_NPCDataStruct NPCData;

	// State Management
	UPROPERTY(BlueprintReadOnly, Category = "State")
	ENPCState CurrentState;

	UPROPERTY(BlueprintReadOnly, Category = "State")
	ENPCState PreviousState;

	// Health System
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth;

	UPROPERTY(BlueprintReadOnly, Category = "Health")
	float MaxHealth;

	// Target Management
	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* CurrentTarget;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	AActor* LastKnownPlayerLocation;

	// Perception
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UAIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UPawnSensingComponent* PawnSensingComponent;

	// Dialogue System
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	bool bIsInDialogue;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FGameplayTag CurrentDialogueNode;
	
protected:
	// Cached Components
	UPROPERTY(VisibleAnywhere, Category="NPC | Dialogue")
	UDialogueComponent* DialogueComponent;


	UPROPERTY(VisibleAnywhere, Category="NPC | AI")
	ANPC_AIController* NPCAIController;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Internal State Management
	UFUNCTION()
	void OnStateChanged(ENPCState OldState, ENPCState NewState);

	// Perception Events
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);

	// Health Events
	UFUNCTION()
	void OnHealthChanged(float NewHealth, float OldHealth);

	UFUNCTION()
	void OnDeath();

	// IInteractionInterface implementation
	virtual void BeginFocus() override;
	virtual void EndFocus() override;
	virtual void BeginInteract() override;
	virtual void EndInteract() override;
	virtual void Interact(AShowcaseProjectCharacter* PlayerCharacter) override;
	
private:
	//Behavior Tree for AI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(AllowPrivateAccess="true"))
	UBehaviorTree *BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(AllowPrivateAccess="true"))
	APatrolPath* PatrolPath;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation", meta=(AllowPrivateAccess="true"))
	UAnimMontage* HookMontage;

	// Timer Handles
	FTimerHandle AlertnessDecayTimer;
	FTimerHandle HealthRegenerationTimer;

	// Internal State
	float CurrentAlertness;
	float TimeInCurrentState;

};
