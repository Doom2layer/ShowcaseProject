// Updated SmartObject.h - Remove unnecessary complexity
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/InteractionInterface.h"
#include "ContextualAnimSceneActorComponent.h"
#include "SmartObjectComponent.h"
#include "SmartObjectTypes.h"
#include "SmartObject.generated.h"

class UStaticMeshComponent;
class USceneComponent;
class UContextualAnimSceneActorComponent;
class USmartObjectComponent;

UCLASS()
class SHOWCASEPROJECT_API ASmartObject : public AActor, public IInteractionInterface
{
    GENERATED_BODY()
    
public:    
    ASmartObject();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* StaticMesh;
    
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UContextualAnimSceneActorComponent* ContextualAnimSceneActor;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USmartObjectComponent* SmartObjectComponent;

    void OnSmartObjectEvent(const FSmartObjectEventData& EventData, const AActor* Interactor) const;

    
    // IInteractionInterface implementation
    virtual void BeginFocus() override;
    virtual void EndFocus() override;
    virtual void BeginInteract() override;
    virtual void EndInteract() override;
    virtual void Interact(AShowcaseProjectCharacter* PlayerCharacter) override;

protected:
    virtual void BeginPlay() override;
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
    
    bool SlotHasTag(FSmartObjectSlotHandle SlotHandle, FGameplayTag Tag, bool bExactMatch = true) const;

    FSmartObjectSlotHandle FindSlotWithTag(FSmartObjectHandle SmartObjectHandle, FGameplayTag RequiredTag, bool bExactMatch = true) const;
    
};