// Copyright Epic Games, Inc. All Rights Reserved.

#include "COMP3013GameMode.h"
#include "COMP3013PlayerController.h"
#include "COMP3013Character.h"
#include "UObject/ConstructorHelpers.h"

ACOMP3013GameMode::ACOMP3013GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ACOMP3013PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownCharacter"));
	if (PlayerPawnBPClass.Class != nullptr)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}

	// set default controller to our Blueprinted controller
	static ConstructorHelpers::FClassFinder<APlayerController> PlayerControllerBPClass(TEXT("/Game/TopDown/Blueprints/BP_TopDownPlayerController"));
	if(PlayerControllerBPClass.Class != NULL)
	{
		PlayerControllerClass = PlayerControllerBPClass.Class;
	}
}