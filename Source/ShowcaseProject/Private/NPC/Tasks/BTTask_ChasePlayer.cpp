// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Tasks/BTTask_ChasePlayer.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NPC/Controller/NPC_AIController.h"

UBTTask_ChasePlayer::UBTTask_ChasePlayer(FObjectInitializer const& ObjectInitializer) : UBTTask_BlackboardBase(ObjectInitializer)
{
	NodeName = TEXT("Chase Player");
}

EBTNodeResult::Type UBTTask_ChasePlayer::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if (ANPC_AIController* const AIController = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
	{
		FVector const PlayerLocation = OwnerComp.GetBlackboardComponent()->GetValueAsVector(GetSelectedBlackboardKey());

		// move to player location
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(AIController, PlayerLocation);

		// finish with success
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
