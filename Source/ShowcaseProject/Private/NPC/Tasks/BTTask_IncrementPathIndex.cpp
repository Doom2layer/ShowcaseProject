// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Tasks/BTTask_IncrementPathIndex.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "NPC/Character/NPC_BaseCharacter.h"
#include "NPC/Controller/NPC_AIController.h"
#include "NPC/Path/PatrolPath.h"

UBTTask_IncrementPathIndex::UBTTask_IncrementPathIndex(FObjectInitializer const& ObjectInitializer) : UBTTask_BlackboardBase(ObjectInitializer)
{
	NodeName = TEXT("Increment Path Index");
}

EBTNodeResult::Type UBTTask_IncrementPathIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	// try and get the AI Controller
	if (ANPC_AIController* const AIController = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
	{
		// try and get the npc
		if (ANPC_BaseCharacter* const NPC = Cast<ANPC_BaseCharacter>(AIController->GetPawn()))
		{
			// Try and get the blackboard
			if (UBlackboardComponent* const BlackboardComponent = OwnerComp.GetBlackboardComponent())
			{
				// get number of patrol points and min and max indices
				int const NoOfPoints = NPC->GetPatrolPath()->Num();
				int const MinIndex = 0;
				int const MaxIndex = NoOfPoints - 1;
				int32 Index = BlackboardComponent->GetValueAsInt(GetSelectedBlackboardKey());

				// Change Direction if we are at the first or last index if we are in bidirectional mode
				if (bBiDirectional)
				{
					if (Index >= MaxIndex && Direction == EDirectionType::Forward)
					{
						Direction = EDirectionType::Backward;
					}
					else if (Index <= MinIndex && Direction == EDirectionType::Backward)
					{
						Direction = EDirectionType::Forward;
					}
				}
				// write new value of index to the blackboard
				UE_LOG(LogTemp, Log, TEXT("BTTask_IncrementPathIndex: Current Index: %d, Direction: %s"), Index, (Direction == EDirectionType::Forward ? TEXT("Forward") : TEXT("Backward")));
				BlackboardComponent->SetValueAsInt(GetSelectedBlackboardKey(),
					(Direction == EDirectionType::Forward ? ++Index : --Index) % NoOfPoints);

				UE_LOG(LogTemp, Log, TEXT("BTTask_IncrementPathIndex: New Index: %d"), BlackboardComponent->GetValueAsInt(GetSelectedBlackboardKey()));
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	// if we reach this point, something went wrong
	UE_LOG(LogTemp, Warning, TEXT("BTTask_IncrementPathIndex: Failed to increment path index!"));
	return EBTNodeResult::Failed;
}
