// STT_UseSmartObject.h
#pragma once

#include "GameplayInteractionContext.h"
#include "Tasks/AITask.h"
#include "STT_UseSmartObject.generated.h"

class AAIController;
class UAITask_MoveTo;
class UGameplayBehavior;
class USmartObjectComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FTaskDelegate, USTT_UseGameplayTask&);

UCLASS(MinimalAPI)
class USTT_UseGameplayTask : public UAITask
{
    GENERATED_BODY()

public:
    explicit USTT_UseGameplayTask(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

    UFUNCTION(BlueprintCallable, Category = "AI|Tasks", meta = (DefaultToSelf = "Controller", BlueprintInternalUseOnly = "true"))
    static USTT_UseGameplayTask* UseSmartObjectWithGameplayInteraction(AAIController* Controller, FSmartObjectClaimHandle ClaimHandle, bool bLockAILogic = true);

    UFUNCTION(BlueprintCallable, Category = "AI|Tasks", meta = (DefaultToSelf = "Controller", BlueprintInternalUseOnly = "true"))
    static USTT_UseGameplayTask* MoveToAndUseSmartObjectWithGameplayInteraction(AAIController* Controller, FSmartObjectClaimHandle ClaimHandle, bool bLockAILogic = true);

    UFUNCTION(BlueprintCallable, Category = "AI|Tasks")
    void RequestAbort();

    void SetShouldReachSlotLocation(const bool bUseMoveTo) { bShouldUseMoveTo = bUseMoveTo; }
    void SetClaimHandle(const FSmartObjectClaimHandle& Handle) { ClaimedHandle = Handle; }

    FTaskDelegate OnFinished;
    FTaskDelegate OnSucceeded;
    FTaskDelegate OnFailed;
    FTaskDelegate OnMoveToFailed;

protected:
    virtual void Activate() override;
    virtual void TickTask(float DeltaTime) override;
    virtual void OnGameplayTaskDeactivated(UGameplayTask& Task) override;
    virtual void OnDestroy(bool bInOwnerFinished) override;

    bool StartInteraction();
    void OnSlotInvalidated(const FSmartObjectClaimHandle& ClaimHandle, const ESmartObjectSlotState State);
    void Abort(EGameplayInteractionAbortReason Reason);

    UPROPERTY()
    FGameplayInteractionContext GameplayInteractionContext;

    UPROPERTY()
    TObjectPtr<UAITask_MoveTo> MoveToTask;

    UPROPERTY()
    FSmartObjectClaimHandle ClaimedHandle;

    UPROPERTY()
    FGameplayInteractionAbortContext AbortContext;

    bool bInteractionCompleted;
    bool bShouldUseMoveTo;
};