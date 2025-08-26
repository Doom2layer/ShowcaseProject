// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/STTask/SmartObjectTasks/STT_FindSmartObjectSlotEntrance.h"
#include "SmartObjectSubsystem.h"


const UStruct* FSTT_FindSmartObjectSlotEntrance::GetInstanceDataType() const
{
	return FSmartObjectSlotEntranceInstanceData::StaticStruct();
}

EStateTreeRunStatus FSTT_FindSmartObjectSlotEntrance::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	USmartObjectSubsystem* SmartObjectSubsystem = USmartObjectSubsystem::GetCurrent(Context.GetWorld());
	if (!SmartObjectSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("SmartObjectSubsystem not found!"));
		return EStateTreeRunStatus::Failed;
	}

	bool EnteranceSlot = SmartObjectSubsystem->FindEntranceLocationForSlot(
		InstanceData.SlotHandle,
		InstanceData.Request,
		InstanceData.Result
	);

	if (!EnteranceSlot)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to find entrance location for slot!"));
		return EStateTreeRunStatus::Failed;
	}

	UE_LOG(LogTemp, Warning, TEXT("Entrance location found: %s"), *InstanceData.Result.Location.ToString());
	// Optionally, you can log the entrance rotation and tags
	UE_LOG(LogTemp, Warning, TEXT("Entrance rotation: %s"), *InstanceData.Result.Rotation.ToString());
	UE_LOG(LogTemp, Warning, TEXT("Entrance tags: %s"), *InstanceData.Result.Tags.ToString());
	
	return EStateTreeRunStatus::Succeeded;
	
}
