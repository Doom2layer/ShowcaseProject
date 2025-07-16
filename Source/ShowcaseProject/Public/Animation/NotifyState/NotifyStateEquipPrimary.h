// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NotifyStateEquipPrimary.generated.h"


class AShowcaseProjectCharacter;
/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UNotifyStateEquipPrimary : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	UPROPERTY()
	AShowcaseProjectCharacter* Character;
	
	void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
};
