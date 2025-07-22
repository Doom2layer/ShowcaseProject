#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GameplayTagContainer.h"
#include "FST_NPCDataStruct.generated.h"

class UTexture2D;
class USkeletalMesh;
class UNPC_AnimInstance;
class USoundBase;

USTRUCT(BlueprintType)
struct FST_NPCDataStruct : public FTableRowBase
{
    GENERATED_BODY()

    // Core Identity
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
    FString NPCName;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
    FString NPCDescription;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
    UTexture2D* NPCPortrait;

    // Faction & Relationship System
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    FGameplayTag FactionTag;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Faction")
    TMap<FGameplayTag, float> FactionRelations; // -1.0 to 1.0 (enemy to ally)

    // Personality Traits (0.0 to 1.0)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Aggressiveness;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Courage;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Curiosity;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Sociability;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Personality", meta = (ClampMin = "0.0", ClampMax = "1.0"))
    float Alertness;

    // Combat Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float MaxHealth;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float WeaponDrawTolerance; // How threatening weapon drawing is (0.0 = panics, 1.0 = ignores)

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float FleeHealthThreshold; // Health percentage to start fleeing

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
    float TakeCoverHealthThreshold; // Health percentage to take cover

    // Behavioral Flags
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bCanSpeak;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bCanFight;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bCanFlee;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Behavior")
    bool bCanTakeCover;

    // Dialogue System
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    UDataTable* DialogueTable;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dialogue")
    FGameplayTag InitialDialogueNode;

    // Visual & Audio
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    USkeletalMesh* NPCMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    TSubclassOf<UNPC_AnimInstance> AnimBlueprintClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presentation")
    TArray<USoundBase*> VoiceLines;

    FST_NPCDataStruct()
    {
        NPCName = TEXT("Generic NPC");
        NPCDescription = TEXT("A generic non-player character");
        
        Aggressiveness = 0.5f;
        Courage = 0.5f;
        Curiosity = 0.5f;
        Sociability = 0.5f;
        Alertness = 0.5f;
        
        MaxHealth = 100.0f;
        WeaponDrawTolerance = 0.5f;
        FleeHealthThreshold = 0.3f;
        TakeCoverHealthThreshold = 0.6f;
        
        bCanSpeak = true;
        bCanFight = false;
        bCanFlee = true;
        bCanTakeCover = true;
    }
};