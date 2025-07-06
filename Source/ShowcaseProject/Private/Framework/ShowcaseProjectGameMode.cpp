// Copyright Epic Games, Inc. All Rights Reserved.

#include "Framework/ShowcaseProjectGameMode.h"
#include "Player/ShowcaseProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AShowcaseProjectGameMode::AShowcaseProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/MainContent/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
