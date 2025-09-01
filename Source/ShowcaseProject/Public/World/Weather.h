// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weather.generated.h"

class ALightning;
class UVolumetricCloudComponent;
class AVolumetricCloud;
class UPostProcessComponent;
class USkyAtmosphereComponent;

UCLASS()
class SHOWCASEPROJECT_API AWeather : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeather();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor, Category="Weather")
	void Dark();

	UFUNCTION(CallInEditor, Category="Weather")
	void Cloudy();

	UFUNCTION(CallInEditor, Category="Weather")
	void ToggleRain();
	
	UFUNCTION(CallInEditor, Category="Weather")
	void Clear();

	UFUNCTION()
	void SetAtmosphere(FLinearColor RayleighScattering, float AutoExposureBias, float StormClouds, float CloudCoverage);

	UFUNCTION()
	void SetAtmosphereTargetParams(FLinearColor TargetRayleighScattering, float TargetAutoExposureBias, float TargetStormClouds, float TargetCloudCoverage);
	
	UPROPERTY(EditAnywhere, Category="Weather")
	USkyAtmosphereComponent* SkyAtmosphereComponent;

	UPROPERTY(EditAnywhere, Category="Weather")
	APostProcessVolume* PostProcessVolume;

	UPROPERTY(EditAnywhere, Category="Weather")
	UVolumetricCloudComponent* VolumetricCloudComponent;

	UPROPERTY()
	UMaterialInstanceDynamic* CloudMaterialInstance;

	UPROPERTY(EditAnywhere, Category="Weather")
	TSubclassOf<ALightning> LightningTemplate;
	
	UPROPERTY(VisibleAnywhere)
	UChildActorComponent* LightningActor;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, Category="Atmosphere Values")
	FLinearColor W_RayleighScattering = FLinearColor(0.411458f, 0.411458f, 0.411458f);
	
	UPROPERTY(VisibleAnywhere, Category="Atmosphere Values")
	float W_AutoExposureBias = 1.0f;
	
	UPROPERTY(VisibleAnywhere, Category="Atmosphere Values")
	float W_StormClouds = 0.0f;
	
	UPROPERTY(VisibleAnywhere, Category="Atmosphere Values")
	float W_CloudCoverage = -0.2f;

	UPROPERTY(VisibleAnywhere, Category="Atmosphere Values")
	FLinearColor W_TargetRayleighScattering;
	
	UPROPERTY(VisibleAnywhere, Category="Atmosphere Values")
	float W_TargetAutoExposureBias;
	
	UPROPERTY(VisibleAnywhere, Category="Atmosphere Values")
	float W_TargetStormClouds;
	
	UPROPERTY(VisibleAnywhere, Category="Atmosphere Values")
	float W_TargetCloudCoverage;
	
	UPROPERTY()
	bool bIsDark;

	UPROPERTY()
	bool bIsCloudy;

	UPROPERTY()
	bool bIsClear;

};
