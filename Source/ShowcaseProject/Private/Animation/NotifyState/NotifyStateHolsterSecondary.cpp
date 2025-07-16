// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyState/NotifyStateHolsterSecondary.h"

#include "Components/WeaponSystemComponent/WeaponSystemComponent.h"
#include "Player/ShowcaseProjectCharacter.h"

void UNotifyStateHolsterSecondary::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                               float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Character = Cast<AShowcaseProjectCharacter>(MeshComp->GetOwner());
	if (Character)
	{
		UE_LOG(LogTemp, Log, TEXT("NotifyStateHolsterSecondary: Begin holster notify for secondary weapon"));
	}
}

void UNotifyStateHolsterSecondary::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	if (MeshComp && MeshComp->GetOwner())
	{
		Character = Cast<AShowcaseProjectCharacter>(MeshComp->GetOwner());
		if (Character)
		{
			if (UWeaponSystemComponent* WeaponSystem = Character->GetWeaponSystem())
			{
				WeaponSystem->OnHolsterAnimationComplete(EWeaponSlot::Secondary);
				UE_LOG(LogTemp, Log, TEXT("NotifyStateHolsterSecondary: End holster notify - secondary weapon holstered"));
			}
		}
	}
}
