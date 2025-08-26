// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC/STTask/STT_MoveToActorOrLocation.h"
#include "StateTreeExecutionContext.h"
#include "AIController.h"
#include "Tasks/AITask_MoveTo.h"

const UStruct* FSTT_MoveToActorOrLocation::GetInstanceDataType() const
{
	return FMoveToActorOrLocationInstanceData::StaticStruct();
}

EStateTreeRunStatus FSTT_MoveToActorOrLocation::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	if (!InstanceData.Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor is not valid!"));
		return EStateTreeRunStatus::Failed;
	}
	if (!InstanceData.TargetActor && InstanceData.TargetLocation.IsZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("TargetActor and TargetLocation are both invalid!"));
		return EStateTreeRunStatus::Failed;
	}
	AAIController* AIController = Cast<AAIController>(InstanceData.Actor);
	if (!AIController)
	{
		UE_LOG(LogTemp, Error, TEXT("No valid AIController found for actor: %s"), 
			   *GetNameSafe(InstanceData.Actor));
		return EStateTreeRunStatus::Failed;
	}

	// Use Controller Move to location

	AIController->MoveToLocation(InstanceData.TargetLocation);

	return EStateTreeRunStatus::Succeeded;

}
