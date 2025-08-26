// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeModule/Public/StateTreeTaskBase.h"
#include "STT_MoveAlongSpline.generated.h"

class ANPC_BaseCharacter;
class ASplineActor;

USTRUCT()
struct FMoveAlongSplineInstanceData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category="Context")
	ANPC_BaseCharacter* Actor;

	UPROPERTY(EditAnywhere, Category="Input")
	ASplineActor* SplineActor;

	int32 NextPatrolPoint;

	FMoveAlongSplineInstanceData()
	{
		Actor = nullptr;
		SplineActor = nullptr;
		NextPatrolPoint = 0;
	}
};

/**
 *
 */
USTRUCT(DisplayName="Move Along Spline", Category="NPC|Tasks")
struct SHOWCASEPROJECT_API FSTT_MoveAlongSpline : public FStateTreeTaskCommonBase
{
	GENERATED_USTRUCT_BODY()

	using FInstanceDataType = FMoveAlongSplineInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

};
