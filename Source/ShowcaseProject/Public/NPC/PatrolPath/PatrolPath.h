// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "NPC/SmartObject/SmartObject.h"
#include "PatrolPath.generated.h"


/**
 * 
 */
UCLASS()
class SHOWCASEPROJECT_API APatrolPath : public ASmartObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APatrolPath();

	FVector GetPatrolPoint(int32 Index) const;
	int Num() const;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol Path", meta = (AllowPrivateAccess = "true", MakeEditWidget = "true"))
	TArray<FVector> PatrolPoints;
};
