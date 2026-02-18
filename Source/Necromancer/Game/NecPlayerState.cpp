#include "Game/NecPlayerState.h"
#include "Component/StatComponent.h"
#include "Component/StaminaComponent.h"
#include "GridInventory/NecInventoryComponent.h"



ANecPlayerState::ANecPlayerState()
{
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	StatComponent->SetIsReplicated(true);

	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	StaminaComponent->SetIsReplicated(true);

	InventoryComponent = CreateDefaultSubobject<UNecInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);
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
		if (NewPS->InventoryComponent) {
			TArray<UItemInstance*> TempItems;
			;
			NewPS->InventoryComponent->LoadItemsFromSaveData(InventoryComponent->GetSavedItems());
		}
	}
}