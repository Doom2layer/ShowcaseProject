// STT_QuerySmartObject.h
#pragma once

#include "CoreMinimal.h"
#include "SmartObjectRequestTypes.h"
#include "SmartObjectSubsystem.h"
#include "StateTreeTaskBase.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "STT_QuerySmartObject.generated.h"

class AAIController;
class USTT_UseGameplayTask;

USTRUCT()
struct FQuerySmartObjectInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, Category="Context")
    TObjectPtr<AAIController> Controller = nullptr;

    // The query template to run
    UPROPERTY(EditAnywhere, Category = Parameter)
    TObjectPtr<UEnvQuery> FindSmartObjectQuery;

    UPROPERTY(EditAnywhere)
    FSmartObjectRequestFilter Filter;

    UPROPERTY(EditAnywhere, Category = Parameter)
    bool bShouldMoveToSlot = true;

    FSmartObjectClaimHandle ClaimHandle;

    // Runtime state tracking
    bool bQueryCompleted = false;
    bool bQuerySucceeded = false;
    bool bTaskCreated = false;
    bool bTaskCompleted = false;
    bool bTaskSucceeded = false;
    int32 QueryID = INDEX_NONE;

    // AI Task for smart object interaction
    UPROPERTY()
    TObjectPtr<USTT_UseGameplayTask> UseSmartObjectTask;
};

USTRUCT(DisplayName="Query Smart Object", Category="Smart Object|Tasks")
struct SHOWCASEPROJECT_API FSTT_QuerySmartObject : public FStateTreeTaskCommonBase
{
    GENERATED_BODY()

    using FInstanceDataType = FQuerySmartObjectInstanceData;

    virtual const UStruct* GetInstanceDataType() const override;
    virtual EStateTreeRunStatus EnterState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;
    virtual EStateTreeRunStatus Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const override;
    virtual void ExitState(FStateTreeExecutionContext& Context, const FStateTreeTransitionResult& Transition) const override;

private:
    void ProcessQueryResultSafe(TSharedPtr<FEnvQueryResult> Result, UWorld* World, FInstanceDataType& InstanceData) const;
    bool StartSmartObjectTask(FInstanceDataType& InstanceData) const;
};