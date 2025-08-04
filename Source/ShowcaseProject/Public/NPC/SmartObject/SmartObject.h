// Updated SmartObject.h - Remove unnecessary complexity
#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "SmartObject.generated.h"

class UBehaviorTree;
class UArrowComponent;
class UBillboardComponent;
class ANPC_BaseCharacter;
class USphereComponent;

UENUM()
enum class ESmartObjectType : uint8
{
    Chair UMETA(DisplayName = "Chair"),
    Bench UMETA(DisplayName = "Bench"),
    PatrolPoint UMETA(DisplayName = "Patrol Point"),
    CoverPoint UMETA(DisplayName = "Cover Point"),
    WorkStation UMETA(DisplayName = "Work Station"),
    Custom UMETA(DisplayName = "Custom"),
};

USTRUCT(BlueprintType)
struct FInteractionSlot
{
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Slot")
    FVector RelativeLocation = FVector::ZeroVector;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Slot")
    FRotator RelativeRotation = FRotator::ZeroRotator;

    UPROPERTY(BlueprintReadOnly, Category = "Interaction Slot")
    bool bIsOccupied = false;

    UPROPERTY(BlueprintReadOnly, Category = "Interaction Slot")
    AActor* CurrentUser = nullptr;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction Slot")
    int32 Priority = 1;

    FInteractionSlot()
    {
        RelativeLocation = FVector::ZeroVector;
        RelativeRotation = FRotator::ZeroRotator;
        bIsOccupied = false;
        CurrentUser = nullptr;
        Priority = 1;
    }
};

UCLASS()
class SHOWCASEPROJECT_API ASmartObject : public AActor, public IInteractionInterface
{
    GENERATED_BODY()
    
public:    
    ASmartObject();

    // Core Smart Object Properties
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Object")
    UBehaviorTree* SubBehaviorTree;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Object")
    ESmartObjectType ObjectType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Object")
    float MaxInteractionDistance = 1500.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Object")
    bool bAllowMultipleUsers = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Object")
    int32 MaxConcurrentUsers = 1;

    // Interaction Slots
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Object", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
    TArray<FInteractionSlot> InteractionSlots;

    // Core Functions
    UFUNCTION(BlueprintCallable, Category = "Smart Object")
    bool CanUserInteract(AActor* User) const;

    UFUNCTION(BlueprintCallable, Category = "Smart Object")
    int32 GetAvailableSlotIndex(AActor* RequestingActor = nullptr) const;

    UFUNCTION(BlueprintCallable, Category = "Smart Object")
    bool ReserveSlot(int32 SlotIndex, AActor* User);

    UFUNCTION(BlueprintCallable, Category = "Smart Object")
    void ReleaseSlotByUser(AActor* User);

    UFUNCTION(BlueprintCallable, Category = "Smart Object")
    FVector GetSlotWorldLocation(int32 SlotIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Smart Object")
    FRotator GetSlotWorldRotation(int32 SlotIndex) const;

    UFUNCTION(BlueprintCallable, Category = "Smart Object")
    int32 GetAvailableSlotCount() const;

    // IInteractionInterface implementation
    virtual void BeginFocus() override;
    virtual void EndFocus() override;
    virtual void BeginInteract() override;
    virtual void EndInteract() override;
    virtual void Interact(AShowcaseProjectCharacter* PlayerCharacter) override;
    int32 GetSlotIndexForUser(AActor* User) const;

protected:
    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Object")
    UBillboardComponent* BillboardComponent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Smart Object")
    UArrowComponent* FacingDirection;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USphereComponent* InteractionSphere;

    // Track users with their assigned slots
    UPROPERTY(BlueprintReadOnly, Category = "Smart Object")
    TMap<AActor*, int32> UserSlotAssignments;

    void GenerateDefaultSlots();
    void UpdateInteractionSphere();

private:
    float GetDistanceToUser(AActor* User) const;
    bool IsUserValid(AActor* User) const;
    void ReleaseSlot(int32 SlotIndex);
};