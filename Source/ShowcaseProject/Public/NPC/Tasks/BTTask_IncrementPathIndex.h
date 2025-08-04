// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_BlackboardBase.h"
#include "BTTask_IncrementPathIndex.generated.h"

UENUM()
enum class EDirectionType
{
	Forward UMETA(DisplayName = "Forward"),
	Backward UMETA(DisplayName = "Backward")
};


/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UBTTask_IncrementPathIndex : public UBTTask_BlackboardBase
{
	GENERATED_BODY()

public:
	explicit UBTTask_IncrementPathIndex(const FObjectInitializer& ObjectInitializer);
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
private:
	EDirectionType Direction = EDirectionType::Forward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta= (AllowPrivateAccess = "true"))
	bool bBiDirectional = false;
	
};
