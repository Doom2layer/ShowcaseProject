// Bench.h
#pragma once

#include "CoreMinimal.h"
#include "NPC/SmartObject/SmartObject.h"
#include "Bench.generated.h"

class UStaticMeshComponent;

UCLASS()
class SHOWCASEPROJECT_API ABench : public ASmartObject
{
	GENERATED_BODY()

public:
	ABench();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* BenchMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bench Settings")
	float SeatSpacing = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Bench Settings")
	float SeatHeight = 50.0f;

private:
	void SetupBenchDefaults();
	void GenerateBenchSlots();
};