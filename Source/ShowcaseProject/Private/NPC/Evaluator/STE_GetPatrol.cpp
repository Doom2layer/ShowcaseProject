// Fill out your copyright notice in the Description page of Project Settings.

#include "NPC/Evaluator/STE_GetPatrol.h"
#include "StateTreeExecutionContext.h"
#include "Kismet/GameplayStatics.h"
#include "NPC/PatrolPath/SplineActor.h"

const UStruct* FSTE_GetPatrol::GetInstanceDataType() const
{
	return FGetPatrolInstanceData::StaticStruct();
}

void FSTE_GetPatrol::TreeStart(FStateTreeExecutionContext& Context) const
{
	FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);
	
	// get actor of all class find the nearest one check if it's a spline actor
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(Context.GetWorld(), ASplineActor::StaticClass(), FoundActors);
	if (FoundActors.Num() > 0)
	{
		// Check distance to the actor
		float MinDistance = FLT_MAX;
		for (AActor* Actor : FoundActors)
		{
				if (Actor->IsA(ASplineActor::StaticClass()))
			{
				float Distance = FVector::Dist(Actor->GetActorLocation(), InstanceData.ActorToEvaluate->GetActorLocation());
				if (Distance < MinDistance)
				{
					MinDistance = Distance;
					InstanceData.NearestSplineActor = Cast<ASplineActor>(Actor);
				}
			}
		}
	}
}
