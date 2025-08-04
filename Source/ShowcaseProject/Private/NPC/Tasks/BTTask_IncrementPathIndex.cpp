// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Tasks/BTTask_IncrementPathIndex.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NPC/Character/NPC_BaseCharacter.h"
#include "NPC/Controller/NPC_AIController.h"
#include "NPC/PatrolPath/PatrolPath.h"

UBTTask_IncrementPathIndex::UBTTask_IncrementPathIndex(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	NodeName = TEXT("Increment Path Index");
}

EBTNodeResult::Type UBTTask_IncrementPathIndex::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	if (ANPC_AIController* Controller = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
	{
		if (ANPC_BaseCharacter* Character = Cast<ANPC_BaseCharacter>(Controller->GetPawn()))
		{
			if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
			{
				// Check if the SmartObject is a PatrolPath
				// if (APatrolPath* PatrolPath = Cast<APatrolPath>(Character->CurrentSmartObject))
				// {
				// 	int const NoOfPoints = PatrolPath->Num();
				// 	int const MinIndex = 0;
				// 	int const MaxIndex = NoOfPoints - 1;
				// 	int Index = Blackboard->GetValueAsInt(GetSelectedBlackboardKey());
				// 	if (bBiDirectional)
				// 	{
				// 		if (Index >= MaxIndex && Direction == EDirectionType::Forward)
				// 		{
				// 			Direction = EDirectionType::Backward;
				// 		}
				// 		else if (Index <= MinIndex && Direction == EDirectionType::Backward)
				// 		{
				// 			Direction = EDirectionType::Forward;
				// 		}
				// 	}
				// 	Blackboard->SetValueAsInt(GetSelectedBlackboardKey(),
				// 		(Direction == EDirectionType::Forward ? ++Index : --Index) % NoOfPoints);
				// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				// 	return EBTNodeResult::Succeeded;
				// }
			}
		}
	}
	return EBTNodeResult::Failed;
}
