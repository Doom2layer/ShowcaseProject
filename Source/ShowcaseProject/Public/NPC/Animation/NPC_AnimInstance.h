// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "NPC_AnimInstance.generated.h"

class UCharacterMovementComponent;
class ANPC_BaseCharacter;
/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API UNPC_AnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	// Used in animation blueprint
	UFUNCTION(BlueprintCallable)
	void UpdateAnimationProperties(float DeltaTime);
	virtual void NativeInitializeAnimation() override;
private:
	// Reference to the owning character
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	ANPC_BaseCharacter* Character;

	// Reference to the character movement component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	UCharacterMovementComponent* CharacterMovement;

	//Ground Speed and Velocity
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	float GroundSpeed;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	FVector Velocity;

	//Direction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	float Direction;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bShouldMove;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Movement", meta=(AllowPrivateAccess="true"))
	bool bIsFalling;
};
