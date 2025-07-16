// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/NotifyState/NotifyStateEquipPrimary.h"
#include "Components/WeaponSystemComponent/WeaponSystemComponent.h"
#include "Player/ShowcaseProjectCharacter.h"

void UNotifyStateEquipPrimary::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                           float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
    Character = Cast<AShowcaseProjectCharacter>(MeshComp->GetOwner());
    if (Character)
    {
        if (UWeaponSystemComponent* WeaponSystem = Character->GetWeaponSystem())
        {
            // This is where the weapon should be attached to the hand
            
        }
    }
}

void UNotifyStateEquipPrimary::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
    const FAnimNotifyEventReference& EventReference)
{
    Character = Cast<AShowcaseProjectCharacter>(MeshComp->GetOwner());
    if (Character)
    {
        if (UWeaponSystemComponent* WeaponSystem = Character->GetWeaponSystem())
        {
            // Animation is cgggomplete
            WeaponSystem->OnEquipAnimationComplete(EWeaponSlot::Primary);
            UE_LOG(LogTemp, Log, TEXT("NotifyStateEquipPrimary: End equip notify - primary weapon equipped"));
        }
    }
}