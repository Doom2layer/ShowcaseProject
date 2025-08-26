// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "StateTreeExecutionContext.h"
#include "StateTreeModule/Public/StateTreeTaskBase.h"
#include "SmartObjectSubsystem.h"
#include "STT_FindSmartObjectSlotEntrance.generated.h"

USTRUCT()
struct FSmartObjectSlotEntranceInstanceData
{
	GENERATED_USTRUCT_BODY()

	UPROPERTY(EditAnywhere, Category="Context")
	FSmartObjectSlotHandle SlotHandle;

	UPROPERTY(EditAnywhere)
	FSmartObjectSlotEntranceLocationRequest Request;

	UPROPERTY(EditAnywhere, Category="Output")
	FSmartObjectSlotEntranceLocationResult Result;

	FSmartObjectSlotEntranceInstanceData()
	{
	}
};

/**
 * 
 */
USTRUCT(DisplayName="Find Smart Objects Slot Entrance", Category="Smart Object|Tasks")
struct SHOWCASEPROJECT_API FSTT_FindSmartObjectSlotEntrance :  public FStateTreeTaskCommonBase
{
	GENERATED_USTRUCT_BODY()
	
	using FInstanceDataType = FSmartObjectSlotEntranceInstanceData;

	virtual const UStruct* GetInstanceDataType() const override;
	
	virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
};
