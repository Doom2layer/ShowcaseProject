// Updated SmartObject.cpp - Simplified implementation
#include "NPC/SmartObject/SmartObject.h"

#include "BlueprintGameplayTagLibrary.h"
#include "SmartObjectComponent.h"
#include "ContextualAnimSceneActorComponent.h"
#include "SmartObjectSubsystem.h"
#include "NPC/Character/NPC_BaseCharacter.h"


ASmartObject::ASmartObject()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	StaticMesh->SetupAttachment(RootComponent);
	SmartObjectComponent = CreateDefaultSubobject<USmartObjectComponent>(TEXT("SmartObjectComponent"));
	SmartObjectComponent->SetupAttachment(StaticMesh);
	ContextualAnimSceneActor = CreateDefaultSubobject<UContextualAnimSceneActorComponent>(TEXT("ContextualAnimSceneActor"));
	ContextualAnimSceneActor->SetupAttachment(RootComponent);
}

void ASmartObject::BeginPlay()
{
	Super::BeginPlay();
	if (SmartObjectComponent)
	{
        SmartObjectComponent->GetOnSmartObjectEventNative().AddLambda([this](const FSmartObjectEventData& EventData, const AActor* Interactor)
        {
            OnSmartObjectEvent(EventData, Interactor);
        });
	}
}

void ASmartObject::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (SmartObjectComponent && SmartObjectComponent->GetOnSmartObjectEventNative().IsBound())
	{
		SmartObjectComponent->GetOnSmartObjectEventNative().RemoveAll(this);
	}
	Super::EndPlay(EndPlayReason);
}

void ASmartObject::OnSmartObjectEvent(const FSmartObjectEventData& EventData, const AActor* Interactor) const
{
	USmartObjectSubsystem* SOSubsystem = USmartObjectSubsystem::GetCurrent(GetWorld());
	if(EventData.Reason == ESmartObjectChangeReason::OnOccupied && SlotHasTag(EventData.SlotHandle, FGameplayTag::RequestGameplayTag(TEXT("SmartObject.Sync.Primary")), true))
	{
		const FSmartObjectSlotHandle Slot = FindSlotWithTag(EventData.SmartObjectHandle, FGameplayTag::RequestGameplayTag(TEXT("SmartObject.Sync.Secondary")), true);
		SOSubsystem->SetSlotEnabled(Slot, true);
		// LOG
		UE_LOG(LogTemp, Warning, TEXT("OnOccupied: Slot %s for SmartObject %s is now enabled"), *LexToString(Slot), *LexToString(EventData.SmartObjectHandle));		
	}
	else if (EventData.Reason == ESmartObjectChangeReason::OnReleased)
	{
		const FSmartObjectSlotHandle Slot = FindSlotWithTag(EventData.SmartObjectHandle, FGameplayTag::RequestGameplayTag(TEXT("SmartObject.Sync.Secondary")), true);
		if (SOSubsystem->GetSlotState(Slot) == ESmartObjectSlotState::Free)
		{
			SOSubsystem->SetSlotEnabled(Slot, false);
			// Log
			UE_LOG(LogTemp, Warning, TEXT("OnReleased: Slot %s for SmartObject %s is now disabled"), *LexToString(Slot), *LexToString(EventData.SmartObjectHandle));
		}
	}
	else if (EventData.Reason == ESmartObjectChangeReason::OnReleased && SlotHasTag(EventData.SlotHandle, FGameplayTag::RequestGameplayTag(TEXT("SmartObject.Sync.Secondary")), true))
	{
		const FSmartObjectSlotHandle Slot = FindSlotWithTag(EventData.SmartObjectHandle, FGameplayTag::RequestGameplayTag(TEXT("SmartObject.Sync.Secondary")), true);
		SOSubsystem->SetSlotEnabled(Slot, false);
		// Log
		UE_LOG(LogTemp, Warning, TEXT("OnReleased: Slot %s for SmartObject %s is now disabled"), *LexToString(Slot), *LexToString(EventData.SmartObjectHandle));
	}
}

bool ASmartObject::	SlotHasTag(FSmartObjectSlotHandle SlotHandle, FGameplayTag Tag, bool bExactMatch) const
{
	if (const USmartObjectSubsystem* SOSubsystem = USmartObjectSubsystem::GetCurrent(GetWorld()))
	{
		return UBlueprintGameplayTagLibrary::HasTag(
			SOSubsystem->GetSlotTags(SlotHandle),
			Tag,
			bExactMatch
		);
	}
	return false;
}

FSmartObjectSlotHandle ASmartObject::FindSlotWithTag(FSmartObjectHandle SmartObjectHandle, FGameplayTag RequiredTag, bool bExactMatch) const
{
	if (const USmartObjectSubsystem* SOSubsystem = USmartObjectSubsystem::GetCurrent(GetWorld()))
	{
		TArray<FSmartObjectSlotHandle> SlotHandles;
		SOSubsystem->GetAllSlots(SmartObjectHandle, SlotHandles);

		for (const FSmartObjectSlotHandle& SlotHandle : SlotHandles)
		{
			if (SlotHasTag(SlotHandle, RequiredTag, bExactMatch))
			{
				return SlotHandle;
			}
		}
	}
	return FSmartObjectSlotHandle(); // Return an invalid handle if no slot with the tag is found
}


void ASmartObject::BeginFocus()
{
	IInteractionInterface::BeginFocus();
}

void ASmartObject::EndFocus()
{
	IInteractionInterface::EndFocus();
}

void ASmartObject::BeginInteract()
{
	IInteractionInterface::BeginInteract();
}

void ASmartObject::EndInteract()
{
	IInteractionInterface::EndInteract();
}

void ASmartObject::Interact(AShowcaseProjectCharacter* PlayerCharacter)
{
	IInteractionInterface::Interact(PlayerCharacter);
}