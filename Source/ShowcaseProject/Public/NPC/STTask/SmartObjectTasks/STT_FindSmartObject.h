// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeModule/Public/StateTreeTaskBase.h"
#include "SmartObjectRequestTypes.h"
#include "SmartObjectSubsystem.h"
#include "STT_FindSmartObject.generated.h"


class ANPC_BaseCharacter;

USTRUCT()
struct FFindSmartObjectInstanceData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category="Context")
	AActor* Actor;

	UPROPERTY(EditAnywhere)
	FSmartObjectRequest Request;

	UPROPERTY(EditAnywhere, Category="Output")
	FSmartObjectClaimHandle ClaimHandle;
	
	FFindSmartObjectInstanceData()
	{
		Actor = nullptr;
		Request.QueryBox = FBox(FVector(-2000.0f), FVector(2000.0f));
		Request.Filter.bShouldEvaluateConditions = true;
		Request.Filter.ClaimPriority = ESmartObjectClaimPriority::Normal;
	}
};
/**
 *
 */
USTRUCT(DisplayName="Find Smart Objects", Category="Smart Object|Tasks")
struct SHOWCASEPROJECT_API FSTT_FindSmartObject : public FStateTreeTaskCommonBase
{
	GENERATED_USTRUCT_BODY()

	using FInstanceDataType = FFindSmartObjectInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

};
