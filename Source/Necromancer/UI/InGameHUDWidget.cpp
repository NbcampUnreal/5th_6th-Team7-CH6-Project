#include "UI/InGameHUDWidget.h"
#include "Game/NecPlayerState.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Component/StatComponent.h"
#include "Component/StaminaComponent.h"
#include "GridInventory/GridInventoryComponent.h"

#include "Character/NecPlayerCharacter.h"

void UInGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitHUD();
}

void UInGameHUDWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0.0f)
	{
		HealthBar->SetPercent(FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f));
	}
}

void UInGameHUDWidget::UpdateStamina(float CurrentStamina, float MaxStamina)
{
	if (StaminaBar && MaxStamina > 0.0f)
	{
		StaminaBar->SetPercent(FMath::Clamp(CurrentStamina / MaxStamina, 0.0f, 1.0f));
	}
}

void UInGameHUDWidget::InitHUD()
{
	ANecPlayerState* PS = GetOwningPlayerState<ANecPlayerState>();
	if (!IsValid(PS))
	{
		return;
	}

	if (!IsValid(PlayerStatComponent))
	{
		PlayerStatComponent = PS->GetStatComponent();
		if (IsValid(PlayerStatComponent))
		{
			PlayerStatComponent->OnHealthChanged.AddDynamic(this, &UInGameHUDWidget::UpdateHealth);
			UpdateHealth(PlayerStatComponent->GetCurrentHealth(), PlayerStatComponent->GetMaxHealth());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InGameHUD: Could not find StatComponent."));
		}
	}

	if (!IsValid(PlayerStaminaComponent))
	{
		PlayerStaminaComponent = PS->GetStaminaComponent();
		if (IsValid(PlayerStaminaComponent))
		{
			PlayerStaminaComponent->OnStaminaChanged.AddDynamic(this, &UInGameHUDWidget::UpdateStamina);
			UpdateStamina(PlayerStaminaComponent->GetCurrentStamina(), PlayerStaminaComponent->GetMaxStamina());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("InGameHUD: Could not find StaminaComponent."));
		}
	}

	APlayerController* PC = Cast<APlayerController>(GetOwningPlayer());
	if (!IsValid(PC))
	{
		return;
	}

	ANecPlayerCharacter* Character = Cast<ANecPlayerCharacter>(PC->GetCharacter());
	if (!IsValid(Character))
	{		return;
	}
	SoulComponent = Character->GetSoulComponent();

	NecInventoryComponent = Character->GetInventoryComponent();
}