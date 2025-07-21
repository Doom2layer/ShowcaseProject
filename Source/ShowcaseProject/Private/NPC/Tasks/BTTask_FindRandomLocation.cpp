// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Tasks/BTTask_FindRandomLocation.h"

#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "NPC/Character/NPC_BaseCharacter.h"

UBTTask_FindRandomLocation::UBTTask_FindRandomLocation(FObjectInitializer const& ObjectInitializer) : UBTTask_BlackboardBase(ObjectInitializer)
{
	NodeName = "Find Random Location";
}

EBTNodeResult::Type UBTTask_FindRandomLocation::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//Get AI Controller and it's NPC
	if (auto* const Controller = Cast<AAIController>(OwnerComp.GetAIOwner()))
	{
		if (auto* const NPC = Cast<ANPC_BaseCharacter>(Controller->GetPawn()))
		{
			auto const Origin = NPC->GetActorLocation();
			if (auto* const NavSys = UNavigationSystemV1::GetCurrent(GetWorld()))
			{
				FNavLocation NavLoc;
				if (NavSys->GetRandomPointInNavigableRadius(Origin, SearchRadius, NavLoc))
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsVector(GetSelectedBlackboardKey(), NavLoc.Location);
				}

				// finish with success
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
				return EBTNodeResult::Succeeded;
			}
		}
	}

	return EBTNodeResult::Failed;
	
}
