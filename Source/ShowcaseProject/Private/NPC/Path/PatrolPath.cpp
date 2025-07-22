// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Path/PatrolPath.h"

// Sets default values
APatrolPath::APatrolPath()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}

FVector APatrolPath::GetPatrolPoint(int32 const index) const
{
	return PatrolPoints.IsValidIndex(index) ? PatrolPoints[index] : FVector::ZeroVector;
}

int APatrolPath::Num() const
{
	return PatrolPoints.Num();
}

