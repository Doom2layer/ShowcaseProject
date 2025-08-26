// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC/STTask/STT_MoveAlongSpline.h"
#include "NPC/PatrolPath/SplineActor.h"
#include "NPC/Character/NPC_BaseCharacter.h"
#include "Components/SplineComponent.h"
#include "NPC/Controller/NPC_AIController.h"

const UStruct* FSTT_MoveAlongSpline::GetInstanceDataType() const
{
	return FMoveAlongSplineInstanceData::StaticStruct();
}

EStateTreeRunStatus FSTT_MoveAlongSpline::EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
    // Defensive programming - validate objects each tick in case they become invalid
    if (!IsValid(InstanceData.Actor))
    {
        UE_LOG(LogTemp, Error, TEXT("STT_MoveAlongSpline: Actor became invalid during execution"));
        return EStateTreeRunStatus::Failed;
    }
    
    if (!IsValid(InstanceData.SplineActor) || !IsValid(InstanceData.SplineActor->SplineComponent))
    {
        UE_LOG(LogTemp, Error, TEXT("STT_MoveAlongSpline: SplineActor or SplineComponent became invalid during execution"));
        return EStateTreeRunStatus::Failed;
    }

    USplineComponent* SplineComp = InstanceData.SplineActor->SplineComponent;

    InstanceData.NextPatrolPoint = FMath::RandRange(0, SplineComp->GetNumberOfSplinePoints() - 1);

    if (ANPC_AIController* controller = InstanceData.Actor->GetController<ANPC_AIController>())
    {
        controller->MoveToLocation(SplineComp->GetLocationAtSplinePoint(InstanceData.NextPatrolPoint, ESplineCoordinateSpace::World));
        UE_LOG(LogTemp, Log, TEXT("STT_MoveAlongSpline: Actor is moving along spline point %d"), InstanceData.NextPatrolPoint);

        return EStateTreeRunStatus::Succeeded;
    }
    UE_LOG(LogTemp, Error, TEXT("STT_MoveAlongSpline: Actor's controller is not an ANPC_AIController or is invalid"));
    return EStateTreeRunStatus::Failed;
    
}

