// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Controller/NPC_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NPC/Character/NPC_BaseCharacter.h"
#include "NPC/StateTree/ShowcaseStateTreeAIComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Perception/AISenseConfig_Hearing.h"

ANPC_AIController::ANPC_AIController(FObjectInitializer const& ObjectInitializer) : Super(ObjectInitializer)
{
	AIPerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComponent"));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	HearingConfig = CreateDefaultSubobject<UAISenseConfig_Hearing>(TEXT("Hearing Config"));
	StateTreeAIComponent = CreateDefaultSubobject<UShowcaseStateTreeAIComponent>(TEXT("StateTreeAIComponent"));

	SetupPerceptionSystem();
}

void ANPC_AIController::BeginPlay()
{
	Super::BeginPlay();

	if (StateTreeAIComponent)
	{
		StateTreeAIComponent->StartLogic();
		UE_LOG(LogTemp, Log, TEXT("AI Controller %s has started logic for NPC %s with state tree %s"), *GetName(), 
			*GetPawn()->GetName(), *StateTreeAIComponent->GetName());  
	}
}

void ANPC_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

    
}

void ANPC_AIController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ANPC_AIController::SetupPerceptionSystem()
{
	if (SightConfig && HearingConfig)
	{
		SightConfig->SightRadius = 2000.0f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 500.0f;
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;

		HearingConfig->HearingRange = 2000.0f;

		// Tell the senses to detect anything
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		HearingConfig->DetectionByAffiliation.bDetectEnemies = true;
		HearingConfig->DetectionByAffiliation.bDetectNeutrals = true;
		HearingConfig->DetectionByAffiliation.bDetectFriendlies = true;

		// Register the sight sense to our perception component
		if (AIPerceptionComponent)
		{
			AIPerceptionComponent->ConfigureSense(*SightConfig);
			AIPerceptionComponent->ConfigureSense(*HearingConfig);
			AIPerceptionComponent->SetDominantSense(SightConfig->GetSenseImplementation());
		}
	}
}
