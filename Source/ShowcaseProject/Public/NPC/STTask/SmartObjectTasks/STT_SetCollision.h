// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeTaskBase.h"
#include "StateTreeExecutionContext.h"
#include "STT_SetCollision.generated.h"

USTRUCT()
struct FSTT_SetCollisionInstanceData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category="Context")
	AActor* Actor;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionChannel> CollisionChannel;

	UPROPERTY(EditAnywhere)
	TEnumAsByte<ECollisionResponse> NewResponse;
	
	ECollisionResponse OriginalResponse;

	FSTT_SetCollisionInstanceData()
	{
		CollisionChannel = ECC_GameTraceChannel2;

		NewResponse = ECR_Ignore;
	}

};

/**
 * 
 */
USTRUCT(DisplayName="Set Smart Object Collision", Category="Smart Object|Utility")
struct SHOWCASEPROJECT_API FSTT_SetCollision : public FStateTreeTaskCommonBase
{
	GENERATED_USTRUCT_BODY()

	using FInstanceDataType = FSTT_SetCollisionInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

	virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

};
