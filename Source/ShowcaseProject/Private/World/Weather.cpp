// Fill out your copyright notice in the Description page of Project Settings.


#include "World/Weather.h"

#include "Components/SkyAtmosphereComponent.h"
#include "Components/VolumetricCloudComponent.h"
#include "World/Lightning.h"

// Sets default values
AWeather::AWeather()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AWeather::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AWeather::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	SetAtmosphereTargetParams(
		{
			FMath::FInterpConstantTo(W_RayleighScattering.R, W_TargetRayleighScattering.R, DeltaTime, 0.1f),
			FMath::FInterpConstantTo(W_RayleighScattering.G, W_TargetRayleighScattering.G, DeltaTime, 0.1f),
			FMath::FInterpConstantTo(W_RayleighScattering.B, W_TargetRayleighScattering.B, DeltaTime, 0.1f),
			FMath::FInterpConstantTo(W_RayleighScattering.A, W_TargetRayleighScattering.A, DeltaTime, 0.1f)
		},
			FMath::FInterpConstantTo(W_AutoExposureBias, W_TargetAutoExposureBias, DeltaTime, 0.1f),
			FMath::FInterpConstantTo(W_StormClouds, W_TargetStormClouds, DeltaTime, 0.1f),
			FMath::FInterpConstantTo(W_CloudCoverage, W_TargetCloudCoverage, DeltaTime, 0.1f)
		);

	if (!FMath::IsNearlyEqual(W_AutoExposureBias, W_TargetAutoExposureBias, 0.05f))
	{
		PrimaryActorTick.bCanEverTick = false;
	}
		
}

void AWeather::Dark()
{
	SetAtmosphere(FLinearColor(0.411458f, 0.411458f, 0.411458f), -1.0f, 1.0f, -0.2f);
	bIsDark = true;
	bIsClear = false;
	bIsCloudy = false;
}

void AWeather::Cloudy()
{
	SetAtmosphere(FLinearColor(0.174647f, 0.40724f, 1.0f), 1.0f, 0.0f, -0.2f);
	bIsDark = false;
	bIsClear = false;
	bIsCloudy = true;
}


void AWeather::Clear()
{
	SetAtmosphere(FLinearColor(0.174647f, 0.40724f, 1.0f), 1.0f, 0.0f, -1.0f);
	bIsDark = false;
	bIsClear = true;
	bIsCloudy = false;
}

void AWeather::ToggleRain()
{
	if (IsValid(LightningActor))
	{
		LightningActor->DestroyChildActor();
		LightningActor = nullptr;
	}

	else
	{
		LightningActor = NewObject<UChildActorComponent>(this, UChildActorComponent::StaticClass(), TEXT("LightningComponent"));
        
		if (IsValid(LightningActor))
		{
			// Configure component properties BEFORE registration
			LightningActor->bEditableWhenInherited = true;
			LightningActor->SetChildActorClass(LightningTemplate);
            
			// Register the component with the world
			LightningActor->RegisterComponent();
            
			// Create the child actor
			LightningActor->CreateChildActor();

			if (ALightning* Lightning = Cast<ALightning>(LightningActor->GetChildActor()))
			{
				Lightning->PostProcessVolume = PostProcessVolume;
			}
		}
	}
}

void AWeather::SetAtmosphere(FLinearColor RayleighScattering, float AutoExposureBias, float StormClouds, float CloudCoverage)
{
	if (!IsValid(SkyAtmosphereComponent) || !IsValid(PostProcessVolume) || !IsValid(VolumetricCloudComponent))
	{
		UE_LOG(LogTemp, Warning, TEXT("SkyAtmosphereComponent, PostProcessComponent or VolumetricCloudComponent is not set in Weather actor."));
		return;
	}

	W_RayleighScattering = RayleighScattering;
	W_AutoExposureBias = AutoExposureBias;
	W_StormClouds = StormClouds;
	W_CloudCoverage = CloudCoverage;
	
	SkyAtmosphereComponent->SetRayleighScattering(RayleighScattering);
	PostProcessVolume->Settings.bOverride_AutoExposureBias = true;
	PostProcessVolume->Settings.AutoExposureBias = AutoExposureBias;
	
	if (IsValid(CloudMaterialInstance))
	{
		CloudMaterialInstance->SetScalarParameterValue("StormClouds", StormClouds);
		CloudMaterialInstance->SetScalarParameterValue("Cloud_GlobalCoverage", CloudCoverage);
		VolumetricCloudComponent->SetMaterial(CloudMaterialInstance);
		return;
	}
	CloudMaterialInstance = UMaterialInstanceDynamic::Create(VolumetricCloudComponent->GetMaterial(), this, "MI_Clouds");
	CloudMaterialInstance->SetScalarParameterValue("StormClouds", StormClouds);
	CloudMaterialInstance->SetScalarParameterValue("Cloud_GlobalCoverage", CloudCoverage);
	VolumetricCloudComponent->SetMaterial(CloudMaterialInstance);
}

void AWeather::SetAtmosphereTargetParams(FLinearColor TargetRayleighScattering, float TargetAutoExposureBias, float TargetStormClouds, float TargetCloudCoverage)
{
	W_TargetRayleighScattering = TargetRayleighScattering;
	W_TargetAutoExposureBias = TargetAutoExposureBias;
	W_TargetStormClouds = TargetStormClouds;
	W_TargetCloudCoverage = TargetCloudCoverage;
	PrimaryActorTick.bCanEverTick = true;
}