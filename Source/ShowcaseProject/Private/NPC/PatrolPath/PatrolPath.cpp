// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/PatrolPath/PatrolPath.h"

APatrolPath::APatrolPath()
{
	PrimaryActorTick.bCanEverTick = false;
}

FVector APatrolPath::GetPatrolPoint(int32 Index) const
{
	return PatrolPoints.IsValidIndex(Index) ? PatrolPoints[Index] : FVector::ZeroVector;
}

int APatrolPath::Num() const
{
	return PatrolPoints.Num();
}
