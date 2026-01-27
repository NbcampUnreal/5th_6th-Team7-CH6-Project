#include "Controller/NecPlayerController.h"
#include "EnhancedInputSubsystems.h"

ANecPlayerController::ANecPlayerController() :
	InputMappingContext(nullptr),
	MoveAction(nullptr),
	LookAction(nullptr),
	SprintAction(nullptr),
	AttackAction(nullptr),
	GuardAction(nullptr),
	LockonAction(nullptr)
{
}

void ANecPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			if (InputMappingContext)
			{
				Subsystem->AddMappingContext(InputMappingContext, 0);
			}
		}
	}
}