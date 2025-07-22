// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_StartDialogue.generated.h"

/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UBTTask_StartDialogue : public UBTTask_BlackboardBase
{
	GENERATED_BODY()
public:
	explicit UBTTask_StartDialogue(FObjectInitializer const& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
};


