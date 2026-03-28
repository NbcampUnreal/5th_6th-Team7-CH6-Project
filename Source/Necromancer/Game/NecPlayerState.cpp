#include "Game/NecPlayerState.h"
#include "Component/StatComponent.h"
#include "Component/StaminaComponent.h"
#include "Component/SoulComponent.h"
#include "GridInventory/NecInventoryComponent.h"
#include "Net/UnrealNetwork.h"


ANecPlayerState::ANecPlayerState()
{
	StatComponent = CreateDefaultSubobject<UStatComponent>(TEXT("StatComponent"));
	StatComponent->SetIsReplicated(true);

	StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	StaminaComponent->SetIsReplicated(true);

	InventoryComponent = CreateDefaultSubobject<UNecInventoryComponent>(TEXT("InventoryComponent"));
	InventoryComponent->SetIsReplicated(true);

	SoulComponent  = CreateDefaultSubobject<USoulComponent>(TEXT("SoulComponent"));
	SoulComponent->SetIsReplicated(true);

	SetNetUpdateFrequency(100.0f);
	SetMinNetUpdateFrequency(30.0f);
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
		if (NewPS->SoulComponent)
		{
			NewPS->SoulComponent->CopySoulDataFrom(SoulComponent);
		}
	}
}

void ANecPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ANecPlayerState, GraceTimeForRevive);
}

void ANecPlayerState::OnRep_GraceTimeForRevive()
{
	APawn* MyPawn = GetPawn();
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3, FColor::Yellow, FString::Printf(TEXT("Gracetime for revive %d"), GraceTimeForRevive));
		OnGraceTimeChanged.Broadcast(GraceTimeForRevive);
	}
}
