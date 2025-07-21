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

protected:
	virtual void OnPossess(APawn* InPawn) override;
private:
	UAISenseConfig_Sight* SightConfig;

	void SetupPerceptionSystem();

	UFUNCTION()
	void OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus);
};
