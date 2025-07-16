// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyState/NotifyStateHolsterPrimary.h"
#include "Components/WeaponSystemComponent/WeaponSystemComponent.h"
#include "Player/ShowcaseProjectCharacter.h"

void UNotifyStateHolsterPrimary::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                             float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Character = Cast<AShowcaseProjectCharacter>(MeshComp->GetOwner());
	if (Character)
	{
            UE_LOG(LogTemp, Log, TEXT("NotifyStateHolsterPrimary: Begin holster notify for primary weapon"));
	}
}

void UNotifyStateHolsterPrimary::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		Character = Cast<AShowcaseProjectCharacter>(MeshComp->GetOwner());
		if (Character)
		{
			if (UWeaponSystemComponent* WeaponSystem = Character->GetWeaponSystem())
			{
				WeaponSystem->OnHolsterAnimationComplete(EWeaponSlot::Primary);
				UE_LOG(LogTemp, Log, TEXT("NotifyStateHolsterPrimary: End holster notify - primary weapon holstered"));
			}
		}
	}
}
