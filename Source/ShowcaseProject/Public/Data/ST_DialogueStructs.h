#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "ST_DialogueStructs.generated.h"

UENUM(BlueprintType)
enum class EDialogueConditionType : uint8
{
    None,
    HasItem,
    QuestStatus,
    FactionRelation,
    PlayerLevel,
    NPCHealth,
    TimeOfDay,
    Custom
};

UENUM(BlueprintType)
enum class EDialogueActionType : uint8
{
    None,
    GiveItem,
    TakeItem,
    StartQuest,
    CompleteQuest,
    ChangeRelation,
    PlayAnimation,
    PlaySound,
    Custom
};

USTRUCT(BlueprintType)
struct FDialogueCondition
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDialogueConditionType ConditionType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag ConditionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float RequiredValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CustomConditionFunction;

    FDialogueCondition()
    {
        ConditionType = EDialogueConditionType::None;
        RequiredValue = 0.0f;
    }
};

USTRUCT(BlueprintType)
struct FDialogueAction
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EDialogueActionType ActionType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag ActionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float ActionValue;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CustomActionFunction;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ActionText; // For display purposes

    FDialogueAction()
    {
        ActionType = EDialogueActionType::None;
        ActionValue = 0.0f;
    }
};

USTRUCT(BlueprintType)
struct FDialogueChoice
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText ChoiceText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag NextNodeTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDialogueCondition> Conditions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDialogueAction> Actions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bEndsDialogue;

    FDialogueChoice()
    {
        bEndsDialogue = false;
    }
};

USTRUCT(BlueprintType)
struct FDialogueNode : public FTableRowBase
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag NodeTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText SpeakerName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText DialogueText;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    USoundBase* VoiceClip;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag EmotionTag; // Happy, Angry, Scared, etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FGameplayTag AnimationTag; // Gesture, Point, Shrug, etc.

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDialogueChoice> Choices;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDialogueCondition> EntryConditions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FDialogueAction> EntryActions;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsPlayerNode; // If true, this is player dialogue

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float AutoAdvanceTime; // Auto advance after X seconds (0 = manual)

    FDialogueNode()
    {
        bIsPlayerNode = false;
        AutoAdvanceTime = 0.0f;
    }
};