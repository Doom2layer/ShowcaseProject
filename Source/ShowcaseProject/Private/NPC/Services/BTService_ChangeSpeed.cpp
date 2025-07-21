// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Services/BTService_ChangeSpeed.h"

#include "AIController.h"
#include "NPC/Character/NPC_BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UBTService_ChangeSpeed::UBTService_ChangeSpeed()
{
	bNotifyBecomeRelevant = true;
	NodeName = TEXT("Change Speed");
}

void UBTService_ChangeSpeed::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);
	if (AAIController* const AIController = OwnerComp.GetAIOwner())
	{
		if (ANPC_BaseCharacter* const NPC = Cast<ANPC_BaseCharacter>(AIController->GetPawn()))
		{
			NPC->GetCharacterMovement()->MaxWalkSpeed = Speed;
		}
	}
}
