// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeModule/Public/StateTreeEvaluatorBase.h"
#include "STE_GetPatrol.generated.h"

class ASplineActor;

USTRUCT()
struct FGetPatrolInstanceData
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, Category="Input")
	AActor* ActorToEvaluate;


	UPROPERTY(EditAnywhere, Category="Output")
	ASplineActor* NearestSplineActor;

	FGetPatrolInstanceData()
	{
		ActorToEvaluate = nullptr;
		NearestSplineActor = nullptr;
	}
};

class ASplineActor;
/**
 * 
 */
USTRUCT()
struct SHOWCASEPROJECT_API FSTE_GetPatrol : public FStateTreeEvaluatorCommonBase
{
	GENERATED_BODY()

	using FInstanceDataType = FGetPatrolInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;
	
	virtual void TreeStart(FStateTreeExecutionContext& Context) const override;

};


