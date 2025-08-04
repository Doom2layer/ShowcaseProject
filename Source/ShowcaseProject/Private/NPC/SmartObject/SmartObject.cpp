// Updated SmartObject.cpp - Simplified implementation
#include "NPC/SmartObject/SmartObject.h"
#include "Components/BillboardComponent.h"
#include "Components/ArrowComponent.h"
#include "Components/SphereComponent.h"
#include "NPC/Character/NPC_BaseCharacter.h"
#include "Player/ShowcaseProjectCharacter.h"



ASmartObject::ASmartObject()
{
    PrimaryActorTick.bCanEverTick = false;

    RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent"));

    BillboardComponent = CreateDefaultSubobject<UBillboardComponent>(TEXT("BillboardComponent"));
    BillboardComponent->SetupAttachment(RootComponent);

    FacingDirection = CreateDefaultSubobject<UArrowComponent>(TEXT("FacingDirection"));
    FacingDirection->SetupAttachment(RootComponent);
    FacingDirection->SetArrowColor(FLinearColor::Blue);

    InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
    InteractionSphere->SetupAttachment(RootComponent);
    InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

    ObjectType = ESmartObjectType::Custom;
    MaxInteractionDistance = 200.0f;
    bAllowMultipleUsers = false;
    MaxConcurrentUsers = 1;
    
}

void ASmartObject::BeginPlay()
{
    Super::BeginPlay();

    if (InteractionSlots.Num() == 0)
    {
        GenerateDefaultSlots();
    }

    UpdateInteractionSphere();
}

bool ASmartObject::CanUserInteract(AActor* User) const
{
    if (!IsUserValid(User))
    {
        UE_LOG(LogTemp, Warning, TEXT("SmartObject %s: Invalid user for interaction"), *GetName());
        return false;
    }

    // Check distance
    if (GetDistanceToUser(User) > MaxInteractionDistance)
    {
        UE_LOG(LogTemp, Warning, TEXT("SmartObject %s: User %s is too far away"), *GetName(), *User->GetName());
        // log distance
        UE_LOG(LogTemp, Log, TEXT("Distance to user: %f, Max allowed: %f, Which results in %s"),
            GetDistanceToUser(User), MaxInteractionDistance,
            (GetDistanceToUser(User) > MaxInteractionDistance) ? TEXT("false") : TEXT("true"));
        return false;
    }

    // Check if there are available slots
    if (GetAvailableSlotCount() == 0 && GetSlotIndexForUser(User) == INDEX_NONE)
    {
        UE_LOG(LogTemp, Warning, TEXT("SmartObject %s: No available slots for user %s"), *GetName(), *User->GetName());
        return false;
    }

    // Check if user already has a slot
    if (GetSlotIndexForUser(User) != INDEX_NONE)
    {
        UE_LOG(LogTemp, Log, TEXT("SmartObject %s: User %s already has a slot assigned"), *GetName(), *User->GetName());
        return true;
    }

    // Check if we have a behavior tree assigned
    if (!SubBehaviorTree)
    {
        UE_LOG(LogTemp, Warning, TEXT("SmartObject %s: No SubBehaviorTree assigned"), *GetName());
        return false;
    }

    return true;
}

int32 ASmartObject::GetAvailableSlotIndex(AActor* RequestingActor) const
{
    // Check if the actor already has a slot
    if (RequestingActor)
    {
        int32 ExistingSlot = GetSlotIndexForUser(RequestingActor);
        if (ExistingSlot != INDEX_NONE)
        {
            return ExistingSlot;
        }
    }

    // Find the best available slot by priority
    int32 BestSlotIndex = INDEX_NONE;
    int32 HighestPriority = 0;

    for (int32 i = 0; i < InteractionSlots.Num(); ++i)
    {
        const FInteractionSlot& Slot = InteractionSlots[i];
        if (!Slot.bIsOccupied && Slot.Priority > HighestPriority)
        {
            BestSlotIndex = i;
            HighestPriority = Slot.Priority;
        }
    }

    return BestSlotIndex;
}

bool ASmartObject::ReserveSlot(int32 SlotIndex, AActor* User)
{
    if (!InteractionSlots.IsValidIndex(SlotIndex) || !User)
    {
        return false;
    }

    FInteractionSlot& Slot = InteractionSlots[SlotIndex];

    if (Slot.bIsOccupied && Slot.CurrentUser != User)
    {
        return false;
    }

    // Release any previous slot for this user
    ReleaseSlotByUser(User);

    // Reserve the new slot
    Slot.bIsOccupied = true;
    Slot.CurrentUser = User;
    UserSlotAssignments.Add(User, SlotIndex);

    UE_LOG(LogTemp, Log, TEXT("SmartObject %s: Slot %d reserved by %s"),
        *GetName(), SlotIndex, *User->GetName());

    return true;
}

void ASmartObject::ReleaseSlotByUser(AActor* User)
{
    if (!User)
    {
        return;
    }

    int32 SlotIndex = GetSlotIndexForUser(User);
    if (SlotIndex != INDEX_NONE)
    {
        ReleaseSlot(SlotIndex);
    }
}

void ASmartObject::ReleaseSlot(int32 SlotIndex)
{
    if (!InteractionSlots.IsValidIndex(SlotIndex))
    {
        return;
    }

    FInteractionSlot& Slot = InteractionSlots[SlotIndex];

    if (Slot.CurrentUser)
    {
        UserSlotAssignments.Remove(Slot.CurrentUser);
        UE_LOG(LogTemp, Log, TEXT("SmartObject %s: Slot %d released by %s"),
            *GetName(), SlotIndex, *Slot.CurrentUser->GetName());
    }

    Slot.bIsOccupied = false;
    Slot.CurrentUser = nullptr;
}

FVector ASmartObject::GetSlotWorldLocation(int32 SlotIndex) const
{
    if (!InteractionSlots.IsValidIndex(SlotIndex))
    {
        return GetActorLocation();
    }

    const FInteractionSlot& Slot = InteractionSlots[SlotIndex];
    return GetActorTransform().TransformPosition(Slot.RelativeLocation);
}

FRotator ASmartObject::GetSlotWorldRotation(int32 SlotIndex) const
{
    if (!InteractionSlots.IsValidIndex(SlotIndex))
    {
        return GetActorRotation();
    }

    const FInteractionSlot& Slot = InteractionSlots[SlotIndex];
    return GetActorTransform().TransformRotation(Slot.RelativeRotation.Quaternion()).Rotator();
}

int32 ASmartObject::GetAvailableSlotCount() const
{
    int32 Count = 0;
    for (const FInteractionSlot& Slot : InteractionSlots)
    {
        if (!Slot.bIsOccupied)
        {
            Count++;
        }
    }
    return Count;
}

void ASmartObject::GenerateDefaultSlots()
{
    InteractionSlots.Empty();

    switch (ObjectType)
    {
    case ESmartObjectType::Bench:
        {
            // Create 3 slots for a bench (left, center, right)
            for (int32 i = 0; i < 3; ++i)
            {
                FInteractionSlot Slot;
                Slot.RelativeLocation = FVector(0.0f, (i - 1) * 100.0f, 0.0f); // Side by side
                Slot.RelativeRotation = FRotator(0.0f, 0.0f, 0.0f); // Facing forward
                Slot.Priority = (i == 1) ? 2 : 1; // Center seat has higher priority
                InteractionSlots.Add(Slot);
            }
            MaxConcurrentUsers = 3;
            bAllowMultipleUsers = true;
        }
        break;

    case ESmartObjectType::Chair:
        {
            FInteractionSlot Slot;
            InteractionSlots.Add(Slot);
            MaxConcurrentUsers = 1;
            bAllowMultipleUsers = false;
        }
        break;

    default:
        {
            FInteractionSlot Slot;
            InteractionSlots.Add(Slot);
            MaxConcurrentUsers = 1;
            bAllowMultipleUsers = false;
        }
        break;
    }

    UE_LOG(LogTemp, Log, TEXT("SmartObject %s: Generated %d slots with proper positioning"), 
        *GetName(), InteractionSlots.Num());
        
    // Log each slot position for debugging
    for (int32 i = 0; i < InteractionSlots.Num(); ++i)
    {
        FVector WorldPos = GetSlotWorldLocation(i);
        UE_LOG(LogTemp, Log, TEXT("  Slot %d: Local(%s) World(%s)"), 
            i, *InteractionSlots[i].RelativeLocation.ToString(), *WorldPos.ToString());
    }
}

void ASmartObject::UpdateInteractionSphere()
{
    if (InteractionSphere)
    {
        InteractionSphere->SetSphereRadius(MaxInteractionDistance);
    }
}

float ASmartObject::GetDistanceToUser(AActor* User) const
{
    if (!User)
    {
        return FLT_MAX;
    }
    return FVector::Dist(GetActorLocation(), User->GetActorLocation());
}

bool ASmartObject::IsUserValid(AActor* User) const
{
    if (!User || !IsValid(User))
    {
        return false;
    }

    // Check if it's an NPC
    if (ANPC_BaseCharacter* NPC = Cast<ANPC_BaseCharacter>(User))
    {
        return NPC->IsAlive() && NPC->GetCurrentState() != ENPCState::Dead;
    }

    // Check if it's the player
    if (Cast<AShowcaseProjectCharacter>(User))
    {
        return true;
    }

    return false;
}

int32 ASmartObject::GetSlotIndexForUser(AActor* User) const
{
    if (const int32* SlotIndex = UserSlotAssignments.Find(User))
    {
        return *SlotIndex;
    }
    return INDEX_NONE;
}

void ASmartObject::BeginFocus() { IInteractionInterface::BeginFocus(); }
void ASmartObject::EndFocus() { IInteractionInterface::EndFocus(); }
void ASmartObject::BeginInteract() { IInteractionInterface::BeginInteract(); }
void ASmartObject::EndInteract() { IInteractionInterface::EndInteract(); }
void ASmartObject::Interact(AShowcaseProjectCharacter* PlayerCharacter) { IInteractionInterface::Interact(PlayerCharacter); }