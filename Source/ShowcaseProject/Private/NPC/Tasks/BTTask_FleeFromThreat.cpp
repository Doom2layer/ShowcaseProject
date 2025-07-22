// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Tasks/BTTask_FleeFromThreat.h"

UBTTask_FleeFromThreat::UBTTask_FleeFromThreat(FObjectInitializer const& ObjectInitializer)
{
}

EBTNodeResult::Type UBTTask_FleeFromThreat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
