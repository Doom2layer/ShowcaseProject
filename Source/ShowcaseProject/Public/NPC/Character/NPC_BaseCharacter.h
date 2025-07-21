// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NPC_BaseCharacter.generated.h"

class APatrolPath;
class UBehaviorTree;
class UInputComponent;

UCLASS()
class SHOWCASEPROJECT_API ANPC_BaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ANPC_BaseCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	FORCEINLINE UBehaviorTree* GetBehaviorTree() const { return BehaviorTree; }

	FORCEINLINE APatrolPath* GetPatrolPath() const { return PatrolPath; }
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Behavior Tree for AI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(AllowPrivateAccess="true"))
	UBehaviorTree *BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", meta=(AllowPrivateAccess="true"))
	APatrolPath* PatrolPath;
	
};
