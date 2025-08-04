// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterface/DialogueWidget/DialogueWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/DialogueComponent/DialogueComponent.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Components/Button.h"
#include "Components/VerticalBoxSlot.h"
#include "UserInterface/ChoiceButton/ChoiceButton.h"

void UDialogueWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Hidden);
	// Initialize state
	bIsDialogueComponentBound = false;
	WeakDialogueComponent.Reset();
	
}

void UDialogueWidget::NativeDestruct()
{
	UE_LOG(LogTemp, Log, TEXT("DialogueWidget: NativeDestruct called"));
	// Critical: Clean up all references before destruction
	CleanupDialogueBinding();
	CleanupChoiceButtons();
	Super::NativeDestruct();
}

void UDialogueWidget::CleanupDialogueBinding()
{
	if (bIsDialogueComponentBound && WeakDialogueComponent.IsValid())
	{
		// Remove all delegate connections to this widget from the dialogue component
		OnChoiceSelected.RemoveAll(this);

		//Also specifically remove the binding to the current component if it exists
		if (UDialogueComponent* DialogueComponent = WeakDialogueComponent.Get())
		{
			OnChoiceSelected.RemoveDynamic(DialogueComponent, &UDialogueComponent::SelectChoice);
			UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Unbound from DialogueComponent %s"), *GetNameSafe(DialogueComponent));
		}
        
		UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Dialogue binding cleaned up"));
	}
	bIsDialogueComponentBound = false;
	WeakDialogueComponent.Reset();
}

void UDialogueWidget::CleanupChoiceButtons()
{
	// Unbind delegates from existing choice buttons
	for (UChoiceButton* Button : ChoiceButtons)
	{
		if (IsValid(Button))
		{
			Button->OnChoiceButtonClicked.RemoveAll(this);
		}
	}

	// Clear the container and array
	if (ChoicesContainer)
	{
		ChoicesContainer->ClearChildren();
	}
    
	ChoiceButtons.Empty();
	UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Choice buttons cleaned up"));
}

void UDialogueWidget::SetDialogueComponent(UDialogueComponent* InDialogueComponent)
{
	// Clean up existing binding first
	CleanupDialogueBinding();
	
	// Validate input before proceeding
	if (!IsValid(InDialogueComponent))
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueWidget: Invalid DialogueComponent provided"));
		CleanupDialogueBinding();
		return;
	}

	// Establish new binding with weak reference
	WeakDialogueComponent = InDialogueComponent;
	bIsDialogueComponentBound = true;

	// Bind delegate ONLY if not already bound to this specific component
	if (WeakDialogueComponent.IsValid())
	{
		// The issue is here - we need to ensure we don't double-bind
		if (!OnChoiceSelected.IsAlreadyBound(InDialogueComponent, &UDialogueComponent::SelectChoice))
		{
			OnChoiceSelected.AddDynamic(InDialogueComponent, &UDialogueComponent::SelectChoice);
			UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Successfully bound to DialogueComponent"));
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("DialogueWidget: Delegate already bound to this component"));
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueWidget: Component became invalid during binding"));
		bIsDialogueComponentBound = false;
	}

}

void UDialogueWidget::DisplayDialogueNode(const FDialogueNode& DialogueNode)
{
	if (!bIsDialogueComponentBound || !WeakDialogueComponent.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueWidget: Cannot display - dialogue component not bound"));
		return;
	}

	SetVisibility(ESlateVisibility::Visible);

	if (SpeakerName && !DialogueNode.SpeakerName.IsEmpty())
	{
		SpeakerName->SetText(DialogueNode.SpeakerName);
	}

	if (DialogueText && !DialogueNode.DialogueText.IsEmpty())
	{
		DialogueText->SetText(DialogueNode.DialogueText);
	}

	CleanupChoiceButtons();

	// Create choice buttons if we have choices
	if (ChoicesContainer && DialogueNode.Choices.Num() > 0)
	{
		// Validate that we have a choice button class configured
		if (!ChoiceButtonClass)
		{
			UE_LOG(LogTemp, Error, TEXT("DialogueWidget: ChoiceButtonClass not set! Please configure it in the Blueprint."));
			return;
		}

		for (int32 i = 0; i < DialogueNode.Choices.Num(); ++i)
		{
			const FDialogueChoice& Choice = DialogueNode.Choices[i];

			// This will now create the Blueprint widget, not the raw C++ class
			if (UChoiceButton* ChoiceButton = CreateWidget<UChoiceButton>(GetWorld(), ChoiceButtonClass))
			{
				ChoiceButton->InitializeChoiceButton(i, Choice.ChoiceText);
				ChoiceButton->OnChoiceButtonClicked.AddDynamic(this, &UDialogueWidget::HandleChoiceButtonClicked);

				ChoicesContainer->AddChild(ChoiceButton);
				ChoiceButtons.Add(ChoiceButton);

				UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Created choice button %d with text: %s"), 
					   i, *Choice.ChoiceText.ToString());
			}
		}

		UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Created %d choice buttons"), ChoiceButtons.Num());
	}

	OnDialogueDisplayed(DialogueNode);
}

void UDialogueWidget::ApplyChoiceButtonStyling(UChoiceButton* ChoiceButton)
{
	if (!ChoiceButton || !ChoicesContainer)
		return;

	// Apply padding to the choice button slot
	if (UPanelSlot* PanelSlot = ChoiceButton->Slot)
	{
		if (UVerticalBoxSlot* VerticalSlot = Cast<UVerticalBoxSlot>(PanelSlot))
		{
			VerticalSlot->SetPadding(ChoiceButtonPadding);
            
			// Set horizontal alignment
			VerticalSlot->SetHorizontalAlignment(HAlign_Fill);
			VerticalSlot->SetVerticalAlignment(VAlign_Top);

			UE_LOG(LogTemp, VeryVerbose, TEXT("DialogueWidget: Applied styling to choice button"));
		}
	}
}

void UDialogueWidget::HideDialogueNode()
{
	SetVisibility(ESlateVisibility::Hidden);
	CleanupChoiceButtons();
	UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Dialogue hidden"));
}

void UDialogueWidget::HandleChoiceButtonClicked(UChoiceButton* ClickedButton, int32 ChoiceIndex)
{
	UE_LOG(LogTemp, Warning, TEXT("=== CHOICE BUTTON SELF-IDENTIFIED: Index %d ==="), ChoiceIndex);

	if (!bIsDialogueComponentBound || !WeakDialogueComponent.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueWidget: Invalid component state"));
		return;
	}

	// Validate button and index match our array
	if (!ChoiceButtons.IsValidIndex(ChoiceIndex) || ChoiceButtons[ChoiceIndex] != ClickedButton)
	{
		UE_LOG(LogTemp, Error, TEXT("DialogueWidget: Button validation failed"));
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("DialogueWidget: Broadcasting validated choice: %d"), ChoiceIndex);
	OnChoiceSelected.Broadcast(ChoiceIndex);

}

// void UDialogueWidget::OnChoiceButtonClicked()
// {
// 	    // Multi-layer validation prevents crashes
//     if (!bIsDialogueComponentBound)
//     {
//         UE_LOG(LogTemp, Error, TEXT("DialogueWidget: Choice clicked but dialogue component not bound"));
//         return;
//     }
//
//     if (!WeakDialogueComponent.IsValid())
//     {
//         UE_LOG(LogTemp, Error, TEXT("DialogueWidget: Choice clicked but dialogue component is invalid"));
//         CleanupDialogueBinding();
//         return;
//     }
//
//     UDialogueComponent* ValidComponent = WeakDialogueComponent.Get();
//     if (!IsValid(ValidComponent))
//     {
//         UE_LOG(LogTemp, Error, TEXT("DialogueWidget: Choice clicked but component failed IsValid check"));
//         CleanupDialogueBinding();
//         return;
//     }
//
//     if (!ValidComponent->bIsInDialogue)
//     {
//         UE_LOG(LogTemp, Warning, TEXT("DialogueWidget: Choice clicked but not in dialogue state"));
//         return;
//     }
//
//     // Find which button was clicked using UE5's widget focus system
//     int32 ClickedChoiceIndex = -1;
//     
//     // Method 1: Check the currently focused widget use UWidget::GetCachedWidget()
// 	if (TSharedPtr<SWidget> FocusedWidget = GetCachedWidget())
// 	{
// 		for (int32 i = 0; i < ChoiceButtons.Num(); ++i)
// 		{
// 			if (ChoiceButtons[i] && ChoiceButtons[i]->GetCachedWidget() == FocusedWidget)
// 			{
// 				ClickedChoiceIndex = i;
// 				UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Found clicked button via focus - Index: %d"), i);
// 				break;
// 			}
// 		}
// 	}
//
//     // Method 2: Fallback - check which button has the "pressed" state
//     if (ClickedChoiceIndex == -1)
//     {
//         for (int32 i = 0; i < ChoiceButtons.Num(); ++i)
//         {
//             if (UChoiceButton* ChoiceButton = ChoiceButtons[i])
//             {
//                 // This assumes your UChoiceButton has a way to detect if it was just clicked
//                 // You might need to implement a temporary flag in UChoiceButton
//                 if (ChoiceButton->IsPressed())
//                 {
//                     ClickedChoiceIndex = i;
//                     UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Found clicked button via state check - Index: %d"), i);
//                     break;
//                 }
//             }
//         }
//     }
//
//     // Method 3: Ultimate fallback - use the stored choice index
//     if (ClickedChoiceIndex == -1)
//     {
//         // This requires modification to your UChoiceButton class
//         for (UChoiceButton* Button : ChoiceButtons)
//         {
//             if (IsValid(Button) && Button->GetChoiceIndex() >= 0)
//             {
//                 ClickedChoiceIndex = Button->GetChoiceIndex();
//                 UE_LOG(LogTemp, Warning, TEXT("DialogueWidget: Using fallback choice detection - Index: %d"), 
//                        ClickedChoiceIndex);
//                 break;
//             }
//         }
//     }
//
//     // Validate the found choice index
//     if (ClickedChoiceIndex >= 0 && ClickedChoiceIndex < ChoiceButtons.Num())
//     {
//         UE_LOG(LogTemp, Log, TEXT("DialogueWidget: Broadcasting choice selection - Index: %d"), ClickedChoiceIndex);
//         
//         // Safe to broadcast choice selection using the delegate
//         if (OnChoiceSelected.IsBound())
//         {
//             OnChoiceSelected.Broadcast(ClickedChoiceIndex);
//         }
//         else
//         {
//             UE_LOG(LogTemp, Error, TEXT("DialogueWidget: OnChoiceSelected delegate not bound"));
//         }
//     }
//     else
//     {
//         UE_LOG(LogTemp, Error, TEXT("DialogueWidget: Could not determine which choice button was clicked. Index: %d, ButtonCount: %d"), 
//                ClickedChoiceIndex, ChoiceButtons.Num());
//     }
//
// }