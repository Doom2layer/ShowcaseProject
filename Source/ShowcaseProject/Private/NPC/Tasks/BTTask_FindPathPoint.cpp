// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Tasks/BTTask_FindPathPoint.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NPC/Character/NPC_BaseCharacter.h"
#include "NPC/Controller/NPC_AIController.h"
#include "NPC/Path/PatrolPath.h"

UBTTask_FindPathPoint::UBTTask_FindPathPoint(FObjectInitializer const& ObjectInitializer) : UBTTask_BlackboardBase(ObjectInitializer)
{
	NodeName = TEXT("Find Path Point");
}

EBTNodeResult::Type UBTTask_FindPathPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//attempt to get the npc's controller
	if(ANPC_AIController* const AIController = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
	{
		// attempt to get the blackboard component from the behavior tree
		if (UBlackboardComponent* const BlackboardComponent = OwnerComp.GetBlackboardComponent())
		{
			//Get the current patrol path index from the blackboard
			int32 const Index = BlackboardComponent->GetValueAsInt(GetSelectedBlackboardKey());

			// get the npc
			if (ANPC_BaseCharacter* const NPC = Cast<ANPC_BaseCharacter>(AIController->GetPawn()))
			{
				// get the current patrol path vector from the NPC - this is local to the patrol path actor
				FVector const PatrolPoint = NPC->GetPatrolPath()->GetPatrolPoint(Index);

				// convert local vector to a world vector
				FVector const WorldPatrolPoint = NPC->GetPatrolPath()->GetActorTransform().TransformPosition(PatrolPoint);

				BlackboardComponent->SetValueAsVector(PatrolPathVectorKey.SelectedKeyName, WorldPatrolPoint);

				// finish with success
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	// if we reach this point, something went wrong
	UE_LOG(LogTemp, Warning, TEXT("BTTask_FindPathPoint: Failed to find path point!"));
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	return EBTNodeResult::Failed;
}
