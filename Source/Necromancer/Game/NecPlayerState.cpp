#include "Game/NecPlayerState.h"
#include "Component/StatComponent.h"
#include "Component/StaminaComponent.h"

ANecPlayerState::ANecPlayerState()
{
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	StatComponent->SetIsReplicated(true);

	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	StaminaComponent->SetIsReplicated(true);
}

void ANecPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);

	ANecPlayerState* NewPS = Cast<ANecPlayerState>(PlayerState);
	if (NewPS)
	{
		if (NewPS->StatComponent && StatComponent)
		{
			NewPS->StatComponent->SetCurrentHealth(StatComponent->GetCurrentHealth());
			// ...
		}
	}
}