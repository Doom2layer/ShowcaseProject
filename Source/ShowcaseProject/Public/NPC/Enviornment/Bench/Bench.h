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
};