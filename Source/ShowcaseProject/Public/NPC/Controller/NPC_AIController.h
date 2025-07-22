// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NPC_AIController.generated.h"

struct FAIStimulus;
class UAISenseConfig_Sight;

/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API ANPC_AIController : public AAIController
{
	GENERATED_BODY()

public:
	explicit ANPC_AIController(FObjectInitializer const& ObjectInitializer);

	// State Management
	UFUNCTION(BlueprintCallable, Category = "AI")
	void UpdateNPCState(ENPCState NewState);

	UFUNCTION(BlueprintCallable, Category = "AI")
	void UpdatePersonalityValues();

	// Target Management
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetCurrentTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "AI")
	AActor* GetCurrentTarget();

	// Location Utilities
	UFUNCTION(BlueprintCallable, Category = "AI")
	FVector FindCoverLocation(FVector ThreatLocation);

	UFUNCTION(BlueprintCallable, Category = "AI")
	FVector FindFleeLocation(FVector ThreatLocation);

	UFUNCTION(BlueprintCallable, Category = "AI")
	bool HasLineOfSight(AActor* Target);
	
	// Blackboard Keys
	static const FName BB_CurrentTarget;
	static const FName BB_CurrentState;
	static const FName BB_PlayerLocation;
	static const FName BB_LastKnownPlayerLocation;
	static const FName BB_PatrolLocation;
	static const FName BB_CoverLocation;
	static const FName BB_FleeLocation;
	static const FName BB_ShouldFlee;
	static const FName BB_ShouldTakeCover;
	static const FName BB_IsInDialogue;
	static const FName BB_Aggressiveness;
	static const FName BB_Courage;
	static const FName BB_Alertness;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UBlackboardComponent* BlackboardComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	class UBehaviorTreeComponent* BehaviorTreeComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComp;
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION()
	void OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors);

	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus);
private:
	UAISenseConfig_Sight* SightConfig;
	class ABaseNPCCharacter* NPCCharacter;
	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
};
