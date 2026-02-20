#include "Character/ARESGameMode.h"
#include "Character/ARESPlayerController.h"
#include "Character/ARESCharacter.h"

AARESGameMode::AARESGameMode()
{
	PlayerControllerClass = AARESPlayerController::StaticClass();
	DefaultPawnClass      = AARESCharacter::StaticClass();
}