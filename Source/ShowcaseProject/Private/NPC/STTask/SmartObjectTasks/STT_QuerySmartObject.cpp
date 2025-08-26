// STT_QuerySmartObject.cpp
#include "NPC/STTask/SmartObjectTasks/STT_QuerySmartObject.h"
#include "NPC/STTask/STT_UseSmartObject.h"
#include "AIController.h"
#include "SmartObjectBlueprintFunctionLibrary.h"
#include "SmartObjectSubsystem.h"
#include "StateTreeExecutionContext.h"
#include "EnvironmentQuery/EnvQueryManager.h"

const UStruct* FSTT_QuerySmartObject::GetInstanceDataType() const
{
    return FQuerySmartObjectInstanceData::StaticStruct();
}

EStateTreeRunStatus FSTT_QuerySmartObject::EnterState(FStateTreeExecutionContext& Context,
    const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

    // Validate prerequisites
    if (!InstanceData.FindSmartObjectQuery)
    {
        UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: No EQS query specified"));
        return EStateTreeRunStatus::Failed;
    }

    if (!InstanceData.Controller || !InstanceData.Controller->GetPawn())
    {
        UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: Invalid controller or pawn"));
        return EStateTreeRunStatus::Failed;
    }

    USmartObjectSubsystem* SmartObjectSubsystem = USmartObjectSubsystem::GetCurrent(Context.GetWorld());
    if (!SmartObjectSubsystem)
    {
        UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: SmartObjectSubsystem not found"));
        return EStateTreeRunStatus::Failed;
    }

    // Reset runtime state
    InstanceData.bQueryCompleted = false;
    InstanceData.bQuerySucceeded = false;
    InstanceData.bTaskCreated = false;
    InstanceData.bTaskCompleted = false;
    InstanceData.bTaskSucceeded = false;
    InstanceData.ClaimHandle = FSmartObjectClaimHandle::InvalidHandle;
    InstanceData.QueryID = INDEX_NONE;
    InstanceData.UseSmartObjectTask = nullptr;

    // Execute EQS query with lambda callback
    FEnvQueryRequest QueryRequest(InstanceData.FindSmartObjectQuery, InstanceData.Controller->GetPawn());

    const FSTT_QuerySmartObject* TaskPtr = this;
    UWorld* World = Context.GetWorld();

    InstanceData.QueryID = QueryRequest.Execute(EEnvQueryRunMode::SingleResult,
        FQueryFinishedSignature::CreateLambda([TaskPtr, World, &InstanceData](TSharedPtr<FEnvQueryResult> Result)
        {
            TaskPtr->ProcessQueryResultSafe(Result, World, InstanceData);
        }));

    if (InstanceData.QueryID == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: Failed to execute EQS query"));
        return EStateTreeRunStatus::Failed;
    }

    UE_LOG(LogTemp, Log, TEXT("STT_QuerySmartObject: Query started with ID %d"), InstanceData.QueryID);
    return EStateTreeRunStatus::Running;
}

EStateTreeRunStatus FSTT_QuerySmartObject::Tick(FStateTreeExecutionContext& Context, const float DeltaTime) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

    // Phase 1: Wait for EQS query completion
    if (!InstanceData.bQueryCompleted)
    {
        return EStateTreeRunStatus::Running;
    }

    if (!InstanceData.bQuerySucceeded)
    {
        UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: Query phase failed"));
        return EStateTreeRunStatus::Failed;
    }

    // Phase 2: Create and start the AI task if not already created
    if (!InstanceData.bTaskCreated && InstanceData.ClaimHandle.IsValid())
    {
        if (!StartSmartObjectTask(InstanceData))
        {
            UE_LOG(LogTemp, Error, TEXT("STT_QuerySmartObject: Failed to start smart object task"));
            return EStateTreeRunStatus::Failed;
        }

        InstanceData.bTaskCreated = true;
        UE_LOG(LogTemp, Log, TEXT("STT_QuerySmartObject: Smart object task created and started"));
    }

    // Phase 3: Wait for task completion
    if (InstanceData.bTaskCreated && !InstanceData.bTaskCompleted)
    {
        // Task is still running - the delegates will update our completion status
        return EStateTreeRunStatus::Running;
    }

    // Phase 4: Return final result
    if (InstanceData.bTaskCompleted)
    {
        const EStateTreeRunStatus FinalResult = InstanceData.bTaskSucceeded ?
            EStateTreeRunStatus::Succeeded : EStateTreeRunStatus::Failed;

        UE_LOG(LogTemp, Log, TEXT("STT_QuerySmartObject: Task completed with result: %s"),
            FinalResult == EStateTreeRunStatus::Succeeded ? TEXT("SUCCESS") : TEXT("FAILURE"));

        return FinalResult;
    }

    return EStateTreeRunStatus::Running;
}

void FSTT_QuerySmartObject::ExitState(FStateTreeExecutionContext& Context,
    const FStateTreeTransitionResult& Transition) const
{
    FInstanceDataType& InstanceData = Context.GetInstanceData<FInstanceDataType>(*this);

    if (InstanceData.QueryID != INDEX_NONE && !InstanceData.bQueryCompleted)
    {
        if (UEnvQueryManager* EQSManager = UEnvQueryManager::GetCurrent(Context.GetWorld()))
        {
            EQSManager->AbortQuery(InstanceData.QueryID);
            UE_LOG(LogTemp, Log, TEXT("STT_QuerySmartObject: Aborted query %d"), InstanceData.QueryID);
        }
    }

    if (InstanceData.UseSmartObjectTask)
    {
        if (!InstanceData.bTaskCompleted)
        {
            InstanceData.UseSmartObjectTask->RequestAbort();
            UE_LOG(LogTemp, Log, TEXT("STT_QuerySmartObject: Requested abort for running task"));
        }

        InstanceData.UseSmartObjectTask->OnFinished.Clear();
        InstanceData.UseSmartObjectTask->OnSucceeded.Clear();
        InstanceData.UseSmartObjectTask->OnFailed.Clear();
        InstanceData.UseSmartObjectTask->OnMoveToFailed.Clear();

        InstanceData.UseSmartObjectTask = nullptr;
    }

    UE_LOG(LogTemp, Log, TEXT("STT_QuerySmartObject: Exit cleanup completed"));
}

bool FSTT_QuerySmartObject::StartSmartObjectTask(FInstanceDataType& InstanceData) const
{
    if (!InstanceData.Controller || !InstanceData.ClaimHandle.IsValid())
    {
        return false;
    }

    // Create the appropriate task based on whether we need to move
    if (InstanceData.bShouldMoveToSlot)
    {
        InstanceData.UseSmartObjectTask = USTT_UseGameplayTask::MoveToAndUseSmartObjectWithGameplayInteraction(
            InstanceData.Controller, InstanceData.ClaimHandle, true);
    }
    else
    {
        InstanceData.UseSmartObjectTask = USTT_UseGameplayTask::UseSmartObjectWithGameplayInteraction(
            InstanceData.Controller, InstanceData.ClaimHandle, true);
    }

    if (!InstanceData.UseSmartObjectTask)
    {
        UE_LOG(LogTemp, Error, TEXT("STT_QuerySmartObject: Failed to create UseGameplayTask"));
        return false;
    }

    // Use lambdas to capture InstanceData reference and match delegate signature
    InstanceData.UseSmartObjectTask->OnFinished.AddLambda([&InstanceData](USTT_UseGameplayTask& Task)
    {
        InstanceData.bTaskCompleted = true;
        UE_LOG(LogTemp, Log, TEXT("STT_QuerySmartObject: Task finished"));
    });

    InstanceData.UseSmartObjectTask->OnSucceeded.AddLambda([&InstanceData](USTT_UseGameplayTask& Task)
    {
        InstanceData.bTaskCompleted = true;
        InstanceData.bTaskSucceeded = true;
        UE_LOG(LogTemp, Log, TEXT("STT_QuerySmartObject: Task succeeded"));
    });

    InstanceData.UseSmartObjectTask->OnFailed.AddLambda([&InstanceData](USTT_UseGameplayTask& Task)
    {
        InstanceData.bTaskCompleted = true;
        InstanceData.bTaskSucceeded = false;
        UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: Task failed"));
    });

    InstanceData.UseSmartObjectTask->OnMoveToFailed.AddLambda([&InstanceData](USTT_UseGameplayTask& Task)
    {
        InstanceData.bTaskCompleted = true;
        InstanceData.bTaskSucceeded = false;
        UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: MoveTo failed"));
    });

    // Start the task
    InstanceData.UseSmartObjectTask->ReadyForActivation();

    return true;
}

void FSTT_QuerySmartObject::ProcessQueryResultSafe(TSharedPtr<FEnvQueryResult> Result,
    UWorld* World, FInstanceDataType& InstanceData) const
{
    if (!IsValid(World))
    {
        UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: World invalid during query completion"));
        InstanceData.bQuerySucceeded = false;
        InstanceData.bQueryCompleted = true;
        return;
    }

    bool bSuccess = false;

    if (Result.IsValid() && Result->IsSuccessful())
    {
        TArray<AActor*> FoundActors;
        Result->GetAllAsActors(FoundActors);

        if (FoundActors.Num() > 0)
        {
            USmartObjectSubsystem* SmartObjectSubsystem = USmartObjectSubsystem::GetCurrent(World);
            if (SmartObjectSubsystem)
            {
                TArray<FSmartObjectRequestResult> SmartObjectResults;

                if (USmartObjectBlueprintFunctionLibrary::FindSmartObjectsInActor(
                    InstanceData.Filter, FoundActors[0], SmartObjectResults, InstanceData.Controller))
                {
                    if (SmartObjectResults.Num() > 0)
                    {
                        const FSmartObjectRequestResult& SelectedResult = SmartObjectResults[0];
                        InstanceData.ClaimHandle = SmartObjectSubsystem->MarkSlotAsClaimed(
                            SelectedResult.SlotHandle, ESmartObjectClaimPriority::Normal);

                        if (InstanceData.ClaimHandle.IsValid())
                        {
                            bSuccess = true;
                            UE_LOG(LogTemp, Log, TEXT("STT_QuerySmartObject: Successfully claimed slot - Handle: %s, Index: %d"),
                                *LexToString(SelectedResult.SlotHandle.GetSmartObjectHandle()),
                                SelectedResult.SlotHandle.GetSlotIndex());
                        }
                        else
                        {
                            UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: Failed to claim slot"));
                        }
                    }
                    else
                    {
                        UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: No smart objects found in actor"));
                    }
                }
                else
                {
                    UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: Actor %s failed to find smart objects"),
                        *FoundActors[0]->GetName());
                }
            }
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: EQS found no actors"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("STT_QuerySmartObject: EQS query failed or returned invalid result"));
    }

    InstanceData.bQuerySucceeded = bSuccess;
    InstanceData.bQueryCompleted = true;

    UE_LOG(LogTemp, Log, TEXT("STT_QuerySmartObject: Query completed with result: %s"),
        bSuccess ? TEXT("SUCCESS") : TEXT("FAILURE"));
}