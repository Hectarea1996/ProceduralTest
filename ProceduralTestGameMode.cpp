// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProceduralTestGameMode.h"
#include "ProceduralTestCharacter.h"
#include "UObject/ConstructorHelpers.h"

AProceduralTestGameMode::AProceduralTestGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
