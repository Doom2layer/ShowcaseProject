#include "World/Lightning.h"
#include "Components/TimelineComponent.h"
#include "Components/PostProcessComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ALightning::ALightning()
{
    PrimaryActorTick.bCanEverTick = false;

    LightningPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LightningPlane"));

    LightningTimeline = CreateDefaultSubobject<UTimelineComponent>(TEXT("LightningTimeline"));
}

void ALightning::BeginPlay()
{
    Super::BeginPlay();

    // Store original exposure
    if (IsValid(PostProcessComponent))
    {
        OriginalExposure = PostProcessComponent->Settings.AutoExposureBias;
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
    if (IsValid(PostProcessComponent))
    {
        PostProcessComponent->Settings.bOverride_AutoExposureBias = true;
        PostProcessComponent->Settings.AutoExposureBias = ExposureIntensity;
        UE_LOG(LogTemp, Warning, TEXT("Exposure adjusted to: %f"), ExposureIntensity);
        GetWorldTimerManager().SetTimer(ExposureResetTimerHandle, this, &ALightning::ResetExposure, ExposureDuration, false);
    }
}

void ALightning::ResetExposure()
{
    if (IsValid(PostProcessComponent))
    {
        PostProcessComponent->Settings.bOverride_AutoExposureBias = true;
        PostProcessComponent->Settings.AutoExposureBias = OriginalExposure;
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

    
    
    const FRotator SpawnRotation(0.0f, UKismetMathLibrary::FindLookAtRotation(SpawnLocation, PlayerLocation).Yaw - 90, 90.0f); // Pitch, Yaw, Roll

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