// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeModule/Public/StateTreeTaskBase.h"
#include "STT_RandomInteger.generated.h"

USTRUCT()
struct FRandomIntegerInstanceData
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(VisibleAnywhere, Category="Output")
	int32 RandomValue;

	UPROPERTY(EditAnywhere)
	int32 MinValue;

	UPROPERTY(EditAnywhere)
	int32 MaxValue;

	FRandomIntegerInstanceData()
	{
		RandomValue = 1;
		MinValue = 1;
		MaxValue = 10;
	}
};

/**
 * 
 */
USTRUCT(DisplayName="Random Integer", Category="AI|Utility")
struct SHOWCASEPROJECT_API FSTT_RandomInteger : public FStateTreeTaskCommonBase
{
	GENERATED_USTRUCT_BODY()

	using FInstanceDataType = FRandomIntegerInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
