	// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/ST_DialogueStructs.h"
#include "DialogueComponent.generated.h"


struct FGameplayTag;
class ANPC_BaseCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SHOWCASEPROJECT_API UDialogueComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDialogueComponent();

	// Current Dialogue State
	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	bool bIsInDialogue;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	AActor* DialoguePartner;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FGameplayTag CurrentNodeTag;

	UPROPERTY(BlueprintReadOnly, Category = "Dialogue")
	FDialogueNode CurrentNode;

	// Dialogue Events
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDialogueStart, AActor*, DialoguePartner, FGameplayTag, StartingNode);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDialogueEnd);
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnDialogueNodeChanged, FGameplayTag, NewNodeTag);

	UPROPERTY(BlueprintAssignable)
	FOnDialogueStart OnDialogueStart;

	UPROPERTY(BlueprintAssignable)
	FOnDialogueEnd OnDialogueEnd;

	UPROPERTY(BlueprintAssignable)
	FOnDialogueNodeChanged OnDialogueNodeChanged;

	// Public Interface
	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool CanStartDialogue(AActor* Initiator);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool StartDialogue(AActor* Partner, FGameplayTag StartingNodeTag);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void EndDialogue();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void SelectChoice(int32 ChoiceIndex);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void AdvanceToNode(FGameplayTag NodeTag);

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	TArray<FDialogueChoice> GetCurrentChoices();

	UFUNCTION(BlueprintPure, Category = "Dialogue")
	FDialogueNode GetCurrentDialogueNode();

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	bool EvaluateCondition(const FDialogueCondition& Condition);

	UFUNCTION(BlueprintCallable, Category = "Dialogue")
	void ExecuteAction(const FDialogueAction& Action);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
private:
	ANPC_BaseCharacter* OwnerNPC;
	FTimerHandle AutoAdvanceTimer;

	UFUNCTION()
	void HandleAutoAdvance();

	void ProcessEntryActions();
	void ProcessExitActions();
	FDialogueNode* GetDialogueNodeByTag(FGameplayTag NodeTag);
};
