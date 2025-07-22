// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Controller/NPC_AIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NPC/Character/NPC_BaseCharacter.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Player/ShowcaseProjectCharacter.h"

ANPC_AIController::ANPC_AIController(FObjectInitializer const& ObjectInitializer)
{
	SetupPerceptionSystem();
}

void ANPC_AIController::BeginPlay()
{
	Super::BeginPlay();
}

void ANPC_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if (ANPC_BaseCharacter* const NPC = Cast<ANPC_BaseCharacter>(InPawn))
	{
		if (UBehaviorTree* const Tree = NPC->GetBehaviorTree())
		{
			UBlackboardComponent* bb;
			UseBlackboard(Tree->BlackboardAsset, bb);
			Blackboard = bb;
			RunBehaviorTree(Tree);
		}
	}
}

void ANPC_AIController::SetupPerceptionSystem()
{
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("Sight Config"));
	if (SightConfig)
	{
		SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(
		TEXT("Perception Component")));
		SightConfig->SightRadius = 500.0f;
		SightConfig->LoseSightRadius = SightConfig->SightRadius + 25.0f;
		SightConfig->PeripheralVisionAngleDegrees = 90.0f;
		SightConfig->SetMaxAge(5.0f);
		SightConfig->AutoSuccessRangeFromLastSeenLocation = 520.0f;
		SightConfig->DetectionByAffiliation.bDetectEnemies = true;
		SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
		SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

		GetPerceptionComponent()->SetDominantSense(*SightConfig->GetSenseImplementation());
		GetPerceptionComponent()->OnTargetPerceptionUpdated.AddDynamic(this, &ANPC_AIController::OnTargetDetected);
		GetPerceptionComponent()->ConfigureSense(*SightConfig);
	}
}

void ANPC_AIController::OnTargetDetected(AActor* Actor, FAIStimulus const Stimulus)
{
	if (AShowcaseProjectCharacter* const Player = Cast<AShowcaseProjectCharacter>(Actor))
	{
		GetBlackboardComponent()->SetValueAsBool("CanSeePlayer", Stimulus.WasSuccessfullySensed());
	}
}

const FName ANPC_AIController::BB_CurrentTarget = TEXT("CurrentTarget");
const FName ANPC_AIController::BB_CurrentState = TEXT("CurrentState");
const FName ANPC_AIController::BB_PlayerLocation = TEXT("PlayerLocation");
const FName ANPC_AIController::BB_LastKnownPlayerLocation = TEXT("LastKnownPlayerLocation");
const FName ANPC_AIController::BB_PatrolLocation = TEXT("PatrolLocation");
const FName ANPC_AIController::BB_CoverLocation = TEXT("CoverLocation");
const FName ANPC_AIController::BB_FleeLocation = TEXT("FleeLocation");
const FName ANPC_AIController::BB_ShouldFlee = TEXT("ShouldFlee");
const FName ANPC_AIController::BB_ShouldTakeCover = TEXT("ShouldTakeCover");
const FName ANPC_AIController::BB_IsInDialogue = TEXT("IsInDialogue");
const FName ANPC_AIController::BB_Aggressiveness = TEXT("Aggressiveness");
const FName ANPC_AIController::BB_Courage = TEXT("Courage");
const FName ANPC_AIController::BB_Alertness = TEXT("Alertness");


void ANPC_AIController::UpdateNPCState(ENPCState NewState)
{
	if (UBlackboardComponent* const BlackboardComponent = GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsEnum(BB_CurrentState, static_cast<uint8>(NewState));
	}
	
	if(ANPC_BaseCharacter* const NPC = Cast<ANPC_BaseCharacter>(GetPawn()))
	{
		NPC->SetNPCState(NewState);
	}
}

void ANPC_AIController::UpdatePersonalityValues()
{
	if (ANPC_BaseCharacter* const NPCChar = Cast<ANPC_BaseCharacter>(GetPawn()))
	{
		if (UBlackboardComponent* const BlackboardComponent = GetBlackboardComponent())
		{
			BlackboardComponent->SetValueAsFloat(BB_Aggressiveness, NPCChar->NPCData.Aggressiveness);
			BlackboardComponent->SetValueAsFloat(BB_Courage, NPCChar->NPCData.Courage);
			BlackboardComponent->SetValueAsFloat(BB_Alertness, NPCChar->NPCData.Alertness);
		}
	}
}


void ANPC_AIController::SetCurrentTarget(AActor* NewTarget)
{
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsObject(BB_CurrentTarget, NewTarget);
	}
}

AActor* ANPC_AIController::GetCurrentTarget()
{
	if (UBlackboardComponent* const BlackboardComponent = GetBlackboardComponent())
	{
		return Cast<AActor>(BlackboardComponent->GetValueAsObject(BB_CurrentTarget));
	}
	return nullptr;
}

FVector ANPC_AIController::FindCoverLocation(FVector ThreatLocation)
{
	
	FVector NPCLocation = GetPawn()->GetActorLocation();
	FVector DirectionAwayFromThreat = (NPCLocation - ThreatLocation).GetSafeNormal();
    
	// Simple cover finding - move away from threat
	FVector CoverLocation = NPCLocation + (DirectionAwayFromThreat * 500.0f);
    
	// You can enhance this with navigation mesh queries for better cover
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsVector(BB_CoverLocation, CoverLocation);
	}
    
	return CoverLocation;
}

FVector ANPC_AIController::FindFleeLocation(FVector ThreatLocation)
{
	FVector NPCLocation = GetPawn()->GetActorLocation();
	FVector FleeDirection = (NPCLocation - ThreatLocation).GetSafeNormal();
    
	// Flee in opposite direction from threat
	FVector FleeLocation = NPCLocation + (FleeDirection * 1000.0f);
    
	if (UBlackboardComponent* BlackboardComponent = GetBlackboardComponent())
	{
		BlackboardComponent->SetValueAsVector(BB_FleeLocation, FleeLocation);
	}
    
	return FleeLocation;
}

bool ANPC_AIController::HasLineOfSight(AActor* Target)
{
	if (!Target || !GetPawn())
	{
		return false;
	}
    
	FVector Start = GetPawn()->GetActorLocation();
	FVector End = Target->GetActorLocation();
    
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetPawn());
	QueryParams.AddIgnoredActor(Target);
    
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECollisionChannel::ECC_Visibility,
		QueryParams
	);
    
	return !bHit; // If no hit, we have line of sight
}


void ANPC_AIController::OnPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
}

void ANPC_AIController::OnTargetPerceptionUpdated(AActor* Actor, struct FAIStimulus Stimulus)
{
}