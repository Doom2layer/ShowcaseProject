// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/STTask/STT_GetRandomLocationInRange.h"
#include "NavigationSystem.h"
#include "StateTreeExecutionContext.h"

const UStruct* FSTT_GetRandomLocationInRange::GetInstanceDataType() const
{
	return FGetRandomLocationInRangeInstanceData::StaticStruct();
}

EStateTreeRunStatus FSTT_GetRandomLocationInRange::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	if (!InstanceData.Actor)
	{
		UE_LOG(LogTemp, Warning, TEXT("Controller is not valid!"));
		return EStateTreeRunStatus::Failed;
	}

	FVector RandomLocation;
	
	if ( UNavigationSystemV1::K2_GetRandomLocationInNavigableRadius(
		Context.GetWorld(),
		InstanceData.Actor->GetActorLocation(),
		RandomLocation,
		InstanceData.Range
	))
	{
		InstanceData.RandomLocation = RandomLocation;
		return EStateTreeRunStatus::Succeeded;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find a random location in range!"));
		return EStateTreeRunStatus::Failed;
	}
	
}
