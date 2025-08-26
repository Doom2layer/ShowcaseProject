// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeModule/Public/StateTreeTaskBase.h"
#include "STT_MoveToActorOrLocation.generated.h"

class ANPC_BaseCharacter;

USTRUCT()
struct FMoveToActorOrLocationInstanceData
{
	GENERATED_USTRUCT_BODY()
	UPROPERTY(EditAnywhere, Category="Context")
	AActor* Actor;

	UPROPERTY(EditAnywhere, Category="Input")
	FVector TargetLocation;

	UPROPERTY(EditAnywhere)
	AActor* TargetActor;

	FMoveToActorOrLocationInstanceData()
		: Actor(nullptr), TargetLocation(FVector::ZeroVector), TargetActor(nullptr)
	{
	}
};

/**
 * 
 */
USTRUCT(DisplayName="Move To Actor Or Location", Category="AI|Tasks")

struct SHOWCASEPROJECT_API FSTT_MoveToActorOrLocation : public FStateTreeTaskCommonBase
{
	GENERATED_USTRUCT_BODY()

	using FInstanceDataType = FMoveToActorOrLocationInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;

	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
