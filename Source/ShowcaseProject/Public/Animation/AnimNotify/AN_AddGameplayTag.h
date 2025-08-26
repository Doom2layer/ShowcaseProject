// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "AN_AddGameplayTag.generated.h"

class IInteractionInterface;
/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UAN_AddGameplayTag : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
		const FAnimNotifyEventReference& EventReference) override;

	UPROPERTY(VisibleAnywhere, Category="Target")
	TScriptInterface<IInteractionInterface> Target;

	UPROPERTY(EditAnywhere, Category="Gameplay Tags")
	FGameplayTag GameplayTagToAdd;

};
