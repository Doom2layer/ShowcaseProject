// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/StateTree/ShowcaseStateTreeComponent.h"

UShowcaseStateTreeComponent::UShowcaseStateTreeComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UShowcaseStateTreeComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UShowcaseStateTreeComponent::BeginPlay()
{
	Super::BeginPlay();
}


void UShowcaseStateTreeComponent::AssignStateTree(UStateTree* NewStateTree)
{
	if (NewStateTree)
	{
		StateTreeRef.SetStateTree(NewStateTree);
		InitializeComponent();
	}
}