// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/DialogueComponent/DialogueComponent.h"
#include "NPC/Character/NPC_BaseCharacter.h"
#include "Player/ShowcaseProjectCharacter.h"
#include "UserInterface/ShowcaseHUD/ShowcaseHUD.h"

// Sets default values for this component's properties
UDialogueComponent::UDialogueComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bIsInDialogue = false;
	DialoguePartner = nullptr;
	CurrentNodeTag = FGameplayTag::EmptyTag;
	OwnerNPC = nullptr;
}


// Called when the game starts
void UDialogueComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerNPC = Cast<ANPC_BaseCharacter>(GetOwner());
}

bool UDialogueComponent::CanStartDialogue(AActor* Initiator)
{
	if (!OwnerNPC || bIsInDialogue)
	{
		//Log OwnerNPC and BIsInDialogue
		UE_LOG(LogTemp, Warning, TEXT("DialogueComponent: Cannot start dialogue. OwnerNPC: %s, bIsInDialogue: %s"), 
			*GetNameSafe(OwnerNPC), bIsInDialogue ? TEXT("true") : TEXT("false"));
	}

	// Check if NPC is in a state that allows dialogue
	ENPCState CurrentState = OwnerNPC->GetCurrentState();
	if (CurrentState == ENPCState::Dead || CurrentState == ENPCState::Combat)
	{
		return false; // NPC cannot engage in dialogue while dead or in combat
	}
	return true;
}

bool UDialogueComponent::StartDialogue(AActor* Partner, FGameplayTag StartingNodeTag)
{
	UE_LOG(LogTemp, Log, TEXT("DialogueComponent: Starting dialogue with %s"), *GetNameSafe(Partner));
	if (!CanStartDialogue(Partner) || !OwnerNPC) return false;
		
	DialoguePartner = Partner;
	bIsInDialogue = true;

	UE_LOG(LogTemp, Log, TEXT("DialogueComponent: Dialogue started with %s"), *GetNameSafe(DialoguePartner));
	
	//Set NPC state to dialogue
	OwnerNPC->SetNPCState(ENPCState::Dialogue);

	UE_LOG(LogTemp, Log, TEXT("DialogueComponent: OwnerNPC %s state set to Dialogue"), *GetNameSafe(OwnerNPC));
	
	//Determine starting node
	FGameplayTag NodeToStart = StartingNodeTag.IsValid() ? StartingNodeTag : OwnerNPC->NPCData.InitialDialogueNode;

	UE_LOG(LogTemp, Log, TEXT("DialogueComponent: Starting node tag is %s"), *NodeToStart.ToString());
	
	AdvanceToNode(NodeToStart);

	OnDialogueStart.Broadcast(DialoguePartner, NodeToStart);

	return true;
}

void UDialogueComponent::EndDialogue()
{
	if (!bIsInDialogue) return;

	ProcessExitActions();

	// Hide dialogue UI
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		if (AShowcaseHUD* HUD = Cast<AShowcaseHUD>(PC->GetHUD()))
		{
			HUD->HideDialogue();
		}
	}
	
	//Clear dialogue state
	bIsInDialogue = false;
	DialoguePartner = nullptr;
	CurrentNodeTag = FGameplayTag::EmptyTag;
	CurrentNode = FDialogueNode();

	//Clear any auto advance timers
	if (GetWorld())
	{
        GetWorld()->GetTimerManager().ClearTimer(AutoAdvanceTimer);
	}

	//return npc to previous state
	if (OwnerNPC)
	{
		OwnerNPC->SetNPCState(OwnerNPC->PreviousState);
	}

	OnDialogueEnd.Broadcast();
}

void UDialogueComponent::SelectChoice(int32 ChoiceIndex)
{
	if (!bIsInDialogue || !CurrentNode.Choices.IsValidIndex(ChoiceIndex)) return;

	FDialogueChoice& SelectedChoice = CurrentNode.Choices[ChoiceIndex];

	//Execute choice actions

	for (const FDialogueAction& Action : SelectedChoice.Actions)
	{
		ExecuteAction(Action);
	}

	// End dialogue if this choice ends it
	if (SelectedChoice.bEndsDialogue)
	{
		EndDialogue();
		return;
	}

	// Move to the next node
	if (SelectedChoice.NextNodeTag.IsValid())
	{
		AdvanceToNode(SelectedChoice.NextNodeTag);
	}
	
}

void UDialogueComponent::AdvanceToNode(FGameplayTag NodeTag)
{
	
	if (!NodeTag.IsValid())
	{
		EndDialogue();
		return;
	}

	FDialogueNode* NewNode = GetDialogueNodeByTag(NodeTag);
	if (!NewNode)
	{
		EndDialogue();
		return;
	}
	//Check entry conditions
	for (const FDialogueCondition& Condition : NewNode->EntryConditions)
	{
		if (!EvaluateCondition(Condition))
		{
			EndDialogue();
			return;
		}
	}
	
	ProcessExitActions();
	CurrentNodeTag = NodeTag;
	CurrentNode = *NewNode;
	ProcessEntryActions();

	
	// Show dialogue in UI
	if (APlayerController* PC = GetWorld()->GetFirstPlayerController())
	{
		UE_LOG(LogTemp, Warning, TEXT("DialogueComponent: Found player controller"));
        
		if (AShowcaseHUD* HUD = Cast<AShowcaseHUD>(PC->GetHUD()))
		{
			UE_LOG(LogTemp, Warning, TEXT("DialogueComponent: Found HUD, calling ShowDialogue"));
			HUD->ShowDialogue(CurrentNode, this);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("DialogueComponent: Could not cast to ShowcaseHUD"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueComponent: Could not find player controller"));
	}
	OnDialogueNodeChanged.Broadcast(NodeTag);

	//Handle auto-advance
	if (CurrentNode.AutoAdvanceTime > 0.0f && GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(AutoAdvanceTimer, this, &UDialogueComponent::HandleAutoAdvance, CurrentNode.AutoAdvanceTime, false);
	}
}

TArray<FDialogueChoice> UDialogueComponent::GetCurrentChoices()
{
	TArray<FDialogueChoice> ValidChoices;

	for (const FDialogueChoice& Choice : CurrentNode.Choices)
	{
		bool bAllConditionsMet = true;

		for (const FDialogueCondition& Condition : Choice.Conditions)
		{
			if (!EvaluateCondition(Condition))
			{
				bAllConditionsMet = false;
				break;
			}
		}
		if (bAllConditionsMet)
		{
			ValidChoices.Add(Choice);
		}
	}
	return ValidChoices;
}

FDialogueNode UDialogueComponent::GetCurrentDialogueNode()
{
	return CurrentNode;
}

bool UDialogueComponent::EvaluateCondition(const FDialogueCondition& Condition)
{
	// Basic condition evaluation - expand based on your needs
	switch (Condition.ConditionType)
	{
	case EDialogueConditionType::None:
		return true; // No condition, always true
	case EDialogueConditionType::HasItem:
		break;
	case EDialogueConditionType::QuestStatus:
		break;
	case EDialogueConditionType::FactionRelation:
		break;
	case EDialogueConditionType::PlayerLevel:
		break;
	case EDialogueConditionType::NPCHealth:
		if (OwnerNPC)
		{
			float HealthPercentage = OwnerNPC->CurrentHealth / OwnerNPC->MaxHealth;
			return HealthPercentage >= Condition.RequiredValue;
		}
		break;
	case EDialogueConditionType::TimeOfDay:
		break;
	case EDialogueConditionType::Custom:
		// Implement custom condition logic
		return true;
	}
	return false; // Default to false if no conditions match
}

void UDialogueComponent::ExecuteAction(const FDialogueAction& Action)
{
	// Basic action execution
	switch (Action.ActionType)
	{
	case EDialogueActionType::None:
		break;
	case EDialogueActionType::GiveItem:
		break;
	case EDialogueActionType::TakeItem:
		break;
	case EDialogueActionType::StartQuest:
		break;
	case EDialogueActionType::CompleteQuest:
		break;
	case EDialogueActionType::ChangeRelation:
		if (OwnerNPC && Action.ActionTag.IsValid())
		{
			// OwnerNPC->SetFactionRelation(Action.ActionTag, Action.ActionValue);
		}
		break;
	case EDialogueActionType::PlayAnimation:
		break;
	case EDialogueActionType::PlaySound:
		break;
	case EDialogueActionType::Custom:
		break;
	}
}

void UDialogueComponent::HandleAutoAdvance()
{
    // Auto-advance to the first available choice or end dialogue
	TArray<FDialogueChoice> ValidChoices = GetCurrentChoices();

	if (ValidChoices.Num() > 0)
	{
		SelectChoice(0);
	}
	else
	{
		EndDialogue();
	}
}

void UDialogueComponent::ProcessEntryActions()
{
	for (const FDialogueAction& Action : CurrentNode.EntryActions)
	{
		ExecuteAction(Action);
	}
}

void UDialogueComponent::ProcessExitActions()
{
	// Process exit actions of current node if any
	// This would be implemented when you have exit actions in your dialogue nodes
}

FDialogueNode* UDialogueComponent::GetDialogueNodeByTag(FGameplayTag NodeTag)
{
	// This assumes you have a data table with dialogue nodes
	// You'll need to set up the data table reference in the component
	if (!OwnerNPC || !OwnerNPC->NPCData.DialogueTable)
	{
		return nullptr;
	}

	return OwnerNPC->NPCData.DialogueTable->FindRow<FDialogueNode>(FName(*NodeTag.ToString()), TEXT(""));

}

