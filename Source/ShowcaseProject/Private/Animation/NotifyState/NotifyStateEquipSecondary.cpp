// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyState/NotifyStateEquipSecondary.h"

#include "Components/WeaponSystemComponent/WeaponSystemComponent.h"
#include "Player/ShowcaseProjectCharacter.h"

void UNotifyStateEquipSecondary::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Character = Cast<AShowcaseProjectCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		UE_LOG(LogTemp, Log, TEXT("NotifyStateEquipSecondary: Begin equip notify for secondary weapon"));
	}
}

void UNotifyStateEquipSecondary::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Character = Cast<AShowcaseProjectCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		if (UWeaponSystemComponent* WeaponSystem = Character->GetWeaponSystem())
		{
			// Animation is cgggomplete
			WeaponSystem->OnEquipAnimationComplete(EWeaponSlot::Secondary);
			UE_LOG(LogTemp, Log, TEXT("NotifyStateEquipSecondary: End equip notify - secondary weapon equipped"));
		}
	}
}
