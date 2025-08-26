// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC/STTask/SmartObjectTasks/STT_FindSmartObject.h"
#include "SmartObjectSubsystem.h"
#include "SmartObjectRequestTypes.h"

const UStruct* FSTT_FindSmartObject::GetInstanceDataType() const
{
	return FFindSmartObjectInstanceData::StaticStruct();
}

EStateTreeRunStatus FSTT_FindSmartObject::EnterState(FStateTreeExecutionContext& Context,
	const FStateTreeTransitionResult& Transition) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

	USmartObjectSubsystem* SmartObjectSubsystem = USmartObjectSubsystem::GetCurrent(Context.GetWorld());
	if (!SmartObjectSubsystem)
	{
		UE_LOG(LogTemp, Warning, TEXT("SmartObjectSubsystem not found!"));
		return EStateTreeRunStatus::Failed;
	}

	TArray<FSmartObjectRequestResult> Results;

	//Log request
	const bool bFoundObjects = SmartObjectSubsystem->FindSmartObjects(InstanceData.Request, Results);
	
	if (!bFoundObjects)
	{
		// Log Request
		return EStateTreeRunStatus::Failed;
	}

	// Process the results
	int32 RandomIndex = FMath::RandRange(0, Results.Num() - 1);
	const FSmartObjectRequestResult& SelectedResult = Results[RandomIndex];
	FSmartObjectClaimHandle Slot = SmartObjectSubsystem->MarkSlotAsClaimed(SelectedResult.SlotHandle, ESmartObjectClaimPriority::Normal, 
		FConstStructView::Make(FSmartObjectActorUserData(InstanceData.Actor)));
	
	UE_LOG(LogTemp, Warning, TEXT("Found Smart Object: %s, Slot Index: %d"),
		*LexToString(SelectedResult.SlotHandle.GetSmartObjectHandle()), SelectedResult.SlotHandle.GetSlotIndex());
	
	if (Slot.IsValid())
	{
		InstanceData.ClaimHandle = Slot;
		// UE_LOG(LogTemp, Warning, TEXT("Smart Object found and claimed successfully!, Slot Index : %d"),
		// InstanceData.ClaimHandle.SlotHandle.GetSlotIndex());
		return EStateTreeRunStatus::Succeeded;
	}
	UE_LOG(LogTemp, Warning, TEXT("Failed to claim Smart Object Slot!"));
	return EStateTreeRunStatus::Failed;
}
