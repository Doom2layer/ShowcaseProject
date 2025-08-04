// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayStateTreeModule/Public/Components/StateTreeComponent.h"
#include "ShowcaseStateTreeComponent.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SHOWCASEPROJECT_API UShowcaseStateTreeComponent : public UStateTreeComponent
{
	GENERATED_BODY()
	
public:
	UShowcaseStateTreeComponent(const FObjectInitializer& ObjectInitializer);
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UFUNCTION(Category="State Tree")
	void AssignStateTree(UStateTree* NewStateTree);

	UFUNCTION(Category="State Tree")
		FORCEINLINE FStateTreeReference GetAssignedStateTree() const { return StateTreeRef; }
	
protected:
	virtual void BeginPlay() override;

	UStateTree* StateTree;
};
