#include "World/Lightning.h"

#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"
#include "Components/TimelineComponent.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/ShowcaseProjectCharacter.h"
#include "World/Weather.h"

ALightning::ALightning()
{
    PrimaryActorTick.bCanEverTick = true;

    PrimaryActorTick.TickInterval = 0.5f;

    SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));

    LightningPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightningPlane"));

    NiagaraRainSystem = CreateDefaultSubobject<UNiagaraComponent>(TEXT("RainParticleSystem"));
    
    LightningTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("LightningTimeline"));

    RainSound = CreateDefaultSubobject<UAudioComponent>(TEXT("RainSound"));

    RootComponent = SceneRoot;

    LightningPlane->SetupAttachment(SceneRoot);

    NiagaraRainSystem->SetupAttachment(SceneRoot);
    
}

void ALightning::Tick(float DeltaSeconds)
{
    if (NiagaraRainSystem)
    {
        // Get the player and set the rain position to the player position
        if (AShowcaseProjectCharacter* PC = Cast<AShowcaseProjectCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))) 
        {
            FVector PlayerLocation = PC->GetActorLocation();
            NiagaraRainSystem->SetWorldLocation({PlayerLocation.X, PlayerLocation.Y, PlayerLocation.Z + 1000.0f});
        }
    }
}

void ALightning::BeginPlay()
{
    Super::BeginPlay();

    if (NiagaraRainSystem)
    {
        NiagaraRainSystem->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
    }

    // Store original exposure
    if (IsValid(PostProcessVolume))
    {
        OriginalExposure = PostProcessVolume->Settings.AutoExposureBias;
        UE_LOG(LogTemp, Warning, TEXT("Original Exposure stored: %f"), OriginalExposure);
    }
    else
    {
        TArray<AActor*> FoundWeathers;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), AWeather::StaticClass(), FoundWeathers);
        for (AActor* WeatherActor : FoundWeathers)
        {
            AWeather* Weather = Cast<AWeather>(WeatherActor);
            if (WeatherActor)
            {
                PostProcessVolume = Weather->PostProcessVolume;
                OriginalExposure = PostProcessVolume->Settings.AutoExposureBias;
                UE_LOG(LogTemp, Warning, TEXT("Original Exposure stored: %f"), OriginalExposure);
                break;
            }
        }
    }
    
    // Setup timeline
    if (LightningCurve)
    {
        FOnTimelineFloat TimelineCallback;
        FOnTimelineEvent TimelineFinishedCallback;

        TimelineCallback.BindUFunction(this, FName("OnTimelineUpdate"));
        TimelineFinishedCallback.BindUFunction(this, FName("OnTimelineFinished"));

        LightningTimeline->AddInterpFloat(LightningCurve, TimelineCallback);
        LightningTimeline->SetTimelineFinishedFunc(TimelineFinishedCallback);
    }

    // Initialize and start
    InitializeMaterial();
    SelectRandomTexture();
    AdjustExposure();
    LightningTimeline->PlayFromStart();
}

void ALightning::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    GetWorldTimerManager().ClearTimer(ReplayTimerHandle);
    GetWorldTimerManager().ClearTimer(ExposureResetTimerHandle);
    ResetExposure();
    Super::EndPlay(EndPlayReason);
}

void ALightning::InitializeMaterial()
{
    if (!IsValid(LightningPlane))
        return;

    UMaterialInterface* BaseMaterial = LightningPlane->GetMaterial(0);
    if (IsValid(BaseMaterial))
    {
        MaterialInstance = UMaterialInstanceDynamic::Create(BaseMaterial, this);
        LightningPlane->SetMaterial(0, MaterialInstance);
    }
}

void ALightning::SelectRandomTexture()
{
    if (LightningTextures.Num() == 0)
    {
        CurrentTextureIndex = INDEX_NONE;
        return;
    }

    CurrentTextureIndex = FMath::RandRange(0, LightningTextures.Num() - 1);

    if (IsValid(MaterialInstance))
    {
        MaterialInstance->SetTextureParameterValue(TEXT("LightningTexture"), LightningTextures[CurrentTextureIndex]);
    }
}

void ALightning::OnTimelineUpdate(float Value)
{
    if (IsValid(MaterialInstance))
    {
        MaterialInstance->SetScalarParameterValue("Y", Value);
        MaterialInstance->SetScalarParameterValue("Emission", Value + 1 * 50.0f);
    }
}

void ALightning::OnTimelineFinished()
{
    if (ReplayDelay > 0.0f)
    {
        GetWorldTimerManager().SetTimer(ReplayTimerHandle, this, &ALightning::StartReplay, ReplayDelay, false);
    }
    else
    {
        StartReplay();
    }
}

void ALightning::StartReplay()
{
    // Position relative to player
    PositionRelativeToPlayer();
    SelectRandomTexture();
    AdjustExposure();
    LightningTimeline->PlayFromStart();
    PlayThunderSound();
}

void ALightning::PlayThunderSound()
{
    //Pick random sound from thunder array and play it
    if (ThunderSounds.Num() > 0)
    {
        int32 RandomIndex = FMath::RandRange(0, ThunderSounds.Num() - 1);
        UGameplayStatics::PlaySoundAtLocation(this, ThunderSounds[RandomIndex], GetActorLocation());
    }
}


void ALightning::AdjustExposure()
{
    if (IsValid(PostProcessVolume))
    {
        PostProcessVolume->Settings.bOverride_AutoExposureBias = true;
        PostProcessVolume->Settings.AutoExposureBias = ExposureIntensity;
        UE_LOG(LogTemp, Warning, TEXT("Exposure adjusted to: %f"), ExposureIntensity);
        GetWorldTimerManager().SetTimer(ExposureResetTimerHandle, this, &ALightning::ResetExposure, ExposureDuration, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("PostProcessComponent is not valid."));
    }
}

void ALightning::ResetExposure()
{
    if (IsValid(PostProcessVolume))
    {
        PostProcessVolume->Settings.bOverride_AutoExposureBias = true;
        PostProcessVolume->Settings.AutoExposureBias = OriginalExposure;
        UE_LOG(LogTemp, Warning, TEXT("Exposure reset to original: %f"), OriginalExposure);
    }
}

void ALightning::PositionRelativeToPlayer()
{
    UWorld* World = GetWorld();
    if (!IsValid(World))
        return;

    APlayerController* PC = World->GetFirstPlayerController();
    if (!IsValid(PC) || !IsValid(PC->GetPawn()))
        return;

    APawn* PlayerPawn = PC->GetPawn();
    const FVector PlayerLocation = PlayerPawn->GetActorLocation();
    const FVector PlayerForward = PlayerPawn->GetActorForwardVector();
    const FVector PlayerRight = PlayerPawn->GetActorRightVector();

    // Random distance and direction
    const float ForwardDistance = FMath::RandRange(3000.0f, 4000.0f);
    const float RightOffset = FMath::RandRange(-2500.0f, 2500.0f);
    const float HeightOffset = FMath::RandRange(500.0f, 1000.0f);

    // Calculate spawn location
    const FVector SpawnLocation = PlayerLocation + 
        (PlayerForward * ForwardDistance) + 
        (PlayerRight * RightOffset) + 
        (FVector::UpVector * HeightOffset);

    
    
    const FRotator SpawnRotation(0.0f, UKismetMathLibrary::FindLookAtRotation(SpawnLocation, PlayerLocation).Yaw - 150, 0.0f); // Pitch, Yaw, Roll

    SetActorLocation(SpawnLocation);
    SetActorRotation(SpawnRotation);
    UE_LOG(LogTemp, Warning, TEXT("Lightning spawned at location: %s"), *SpawnLocation.ToString());
}

ALightning* ALightning::SpawnNearPlayer(UWorld* World)
{
    if (!IsValid(World))
        return nullptr;

    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

    return World->SpawnActor<ALightning>(ALightning::StaticClass(), FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);
}