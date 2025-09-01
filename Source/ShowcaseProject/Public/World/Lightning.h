#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/PostProcessComponent.h"
#include "Lightning.generated.h"

class UNiagaraComponent;
class UTimelineComponent;

UCLASS()
class SHOWCASEPROJECT_API ALightning : public AActor
{
	GENERATED_BODY()

public:
	ALightning();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category="Lightning | Scene")
	USceneComponent* SceneRoot;

	UPROPERTY(EditAnywhere, Category="Lightning | Sound")
	UAudioComponent* RainSound;

	UPROPERTY(EditAnywhere, Category="Lightning | Sound")
	TArray<USoundBase*> ThunderSounds;
	
	UPROPERTY(EditAnywhere, Category="Lightning | Rain")
	UNiagaraComponent* NiagaraRainSystem;
	
	UPROPERTY(EditAnywhere, Category="Lightning")
	TArray<UTexture2D*> LightningTextures;

	UPROPERTY(EditAnywhere, Category="Lightning")
	UCurveFloat* LightningCurve;

	UPROPERTY(EditAnywhere, Category="Lightning")
	float ReplayDelay = 2.0f;

	UPROPERTY(EditAnywhere, Category="Lightning")
	float ExposureIntensity = 11.0f;

	UPROPERTY(EditAnywhere, Category="Lightning")
	float ExposureDuration = 0.2f;

	float OriginalExposure = 0.0f;

	UPROPERTY(EditAnywhere, Category="Lightning")
	APostProcessVolume* PostProcessVolume;
	
	static ALightning* SpawnNearPlayer(UWorld* World);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* LightningPlane;

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* LightningTimeline;

	UPROPERTY()
	UMaterialInstanceDynamic* MaterialInstance;

	FTimerHandle ReplayTimerHandle;
	FTimerHandle ExposureResetTimerHandle;
	int32 CurrentTextureIndex = INDEX_NONE;

	UFUNCTION()
	void OnTimelineUpdate(float Value);

	UFUNCTION()
	void OnTimelineFinished();

	UFUNCTION()
	void StartReplay();

	UFUNCTION()
	void ResetExposure();

	UFUNCTION()
	void PlayThunderSound();

	void InitializeMaterial();
	void SelectRandomTexture();
	void AdjustExposure();
	void PositionRelativeToPlayer();
};