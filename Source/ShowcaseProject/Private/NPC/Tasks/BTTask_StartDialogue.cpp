// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Tasks/BTTask_StartDialogue.h"

UBTTask_StartDialogue::UBTTask_StartDialogue(FObjectInitializer const& ObjectInitializer)
{
}

EBTNodeResult::Type UBTTask_StartDialogue::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	return Super::ExecuteTask(OwnerComp, NodeMemory);
}
