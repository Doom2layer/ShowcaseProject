// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC/STTask/SmartObjectTasks/STT_RandomInteger.h"
#include "StateTreeExecutionContext.h"

const UStruct* FSTT_RandomInteger::GetInstanceDataType() const
{
	return FRandomIntegerInstanceData::StaticStruct();
}

EStateTreeRunStatus FSTT_RandomInteger::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	InstanceData.RandomValue = FMath::RandRange(InstanceData.MinValue, InstanceData.MaxValue);
	return EStateTreeRunStatus::Succeeded;
}
