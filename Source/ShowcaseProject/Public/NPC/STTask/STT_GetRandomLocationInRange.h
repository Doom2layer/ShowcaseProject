// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"

#include "STT_GetRandomLocationInRange.generated.h"

USTRUCT()
struct FGetRandomLocationInRangeInstanceData
{
	
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category="Context")
	TObjectPtr<AActor> Actor = nullptr;

	UPROPERTY(EditAnywhere)
	float Range = 15000.0f;

	UPROPERTY(EditAnywhere, Category="Output")
	FVector RandomLocation = FVector::ZeroVector;
	
	FGetRandomLocationInRangeInstanceData()
	{
		
	};
	
};

/**
 * 
 */
USTRUCT(DisplayName="Get Random Location In Range", Category="AI|Tasks")
struct SHOWCASEPROJECT_API FSTT_GetRandomLocationInRange : public FStateTreeTaskCommonBase
{
	GENERATED_USTRUCT_BODY()

	using FInstanceDataType = FGetRandomLocationInRangeInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
	
};
