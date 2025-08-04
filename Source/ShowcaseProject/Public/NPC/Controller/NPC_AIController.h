// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "NPC_AIController.generated.h"

struct FCachedBlackboardData;
class ANPC_BaseCharacter;
class UAISenseConfig_Sight;
class UAISenseConfig_Hearing;
struct FAIStimulus;

/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API ANPC_AIController : public AAIController
{
	GENERATED_BODY()

public:
	explicit ANPC_AIController(FObjectInitializer const& ObjectInitializer);

	
	UPROPERTY(BlueprintReadWrite)
	float DetectionLevel = 0.0f;

	UPROPERTY(BlueprintReadWrite)
	float TimeStamp = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UAIPerceptionComponent* AIPerceptionComponent;

protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaSeconds) override;
	
	UAISenseConfig_Sight* SightConfig;
	UAISenseConfig_Hearing* HearingConfig;

private:
	ANPC_BaseCharacter* PossessedNPCCharacter;
	void SetupPerceptionSystem();

};
