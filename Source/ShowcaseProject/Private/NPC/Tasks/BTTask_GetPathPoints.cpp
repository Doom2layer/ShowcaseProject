// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Tasks/BTTask_GetPathPoints.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyType_Vector.h"
#include "NPC/Controller/NPC_AIController.h"
#include "NPC/Character/NPC_BaseCharacter.h"
#include "NPC/PatrolPath/PatrolPath.h"

UBTTask_GetPathPoints::UBTTask_GetPathPoints(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	bCreateNodeInstance = true;
	NodeName = TEXT("Get Path Points");
}

EBTNodeResult::Type UBTTask_GetPathPoints::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{

	if (ANPC_AIController* Controller = Cast<ANPC_AIController>(OwnerComp.GetAIOwner()))
	{
		if (ANPC_BaseCharacter* NPCCharacter = Cast<ANPC_BaseCharacter>(Controller->GetPawn()))
		{
			if (UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent())
			{
				// Check if the smart object is a PatrolPath
				// if (APatrolPath* PatrolPath = Cast<APatrolPath>(NPCCharacter->CurrentSmartObject))
				// {
				// 	int32 const Index = Blackboard->GetValueAsInt(GetSelectedBlackboardKey());
				//
				// 	FVector PathPoint = PatrolPath->GetPatrolPoint(Index);
				//
				// 	UE::Math::TVector<double> const GlobalPoint = PatrolPath->GetActorTransform().TransformPosition(PathPoint);
				//
				// 	Blackboard->SetValueAsVector(MoveToLocationVectorKey.SelectedKeyName, GlobalPoint);
				//
				// 	FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				// 	return EBTNodeResult::Succeeded;
				// }
			}
		}
	}
	return EBTNodeResult::Failed;
}
