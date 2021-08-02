// Copyright Epic Games, Inc. All Rights Reserved.

#include "TimeReversalGameMode.h"
#include "TimeReversalHUD.h"
#include "TimeReversalCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATimeReversalGameMode::ATimeReversalGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ATimeReversalHUD::StaticClass();
}
