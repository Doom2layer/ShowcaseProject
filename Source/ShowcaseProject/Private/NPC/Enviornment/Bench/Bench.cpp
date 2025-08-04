// Fill out your copyright notice in the Description page of Project Settings.


#include "NPC/Enviornment/Bench/Bench.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/Engine.h"

ABench::ABench()
{
	// Create the bench mesh component
	BenchMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BenchMesh"));
	BenchMesh->SetupAttachment(RootComponent);
	BenchMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	BenchMesh->SetCollisionResponseToAllChannels(ECR_Block);

	SetupBenchDefaults();
}

void ABench::BeginPlay()
{
	Super::BeginPlay();
    
	// Generate bench-specific slots if none were manually configured
	if (InteractionSlots.Num() == 0)
	{
		GenerateBenchSlots();
	}
}

void ABench::SetupBenchDefaults()
{
	// Configure smart object settings specific to benches
	ObjectType = ESmartObjectType::Bench;
	bAllowMultipleUsers = true;
	MaxConcurrentUsers = 3;
	MaxInteractionDistance = 150.0f;
}

void ABench::GenerateBenchSlots()
{
	InteractionSlots.Empty();
    
	// Generate 3 slots along the bench
	for (int32 i = 0; i < MaxConcurrentUsers; ++i)
	{
		FInteractionSlot Slot;
        
		// Position slots along the bench's local Y-axis
		float YOffset = (i - 1) * SeatSpacing; // Center seat at 0, left at -100, right at +100
		Slot.RelativeLocation = FVector(0.0f, YOffset, SeatHeight);
        
		// All seats face forward (same direction as the bench)
		Slot.RelativeRotation = FRotator::ZeroRotator;
        
		// Middle seat has highest priority
		Slot.Priority = (i == 1) ? 3 : (i == 0 ? 2 : 1);
        
		InteractionSlots.Add(Slot);
	}
    
	UE_LOG(LogTemp, Log, TEXT("Bench %s: Generated %d interaction slots"), 
		   *GetName(), InteractionSlots.Num());
}