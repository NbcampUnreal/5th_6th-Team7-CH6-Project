#include "UI/InGameHUDWidget.h"
#include "Game/NecPlayerState.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Component/StatComponent.h"
#include "Component/StaminaComponent.h"

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
	if (PlayerStatComponent && PlayerStaminaComponent)
	{
		return;
	}

	ANecPlayerState* PS = GetOwningPlayerState<ANecPlayerState>();
	if (!PS)
	{
		return;
	}

	PlayerStatComponent = PS->GetStatComponent();
	if (IsValid(PlayerStatComponent))
	{
		PlayerStatComponent->OnHealthChanged.AddDynamic(this, &UInGameHUDWidget::UpdateHealth);
		UpdateHealth(PlayerStatComponent->GetCurrentHealth(), PlayerStatComponent->GetMaxHealth());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InGameHUD: Could not found StatComponent."));
		return;
	}

	PlayerStaminaComponent = PS->GetStaminaComponent();
	if (IsValid(PlayerStaminaComponent))
	{
		PlayerStaminaComponent->OnStaminaChanged.AddDynamic(this, &UInGameHUDWidget::UpdateStamina);
		UpdateStamina(PlayerStaminaComponent->GetCurrentStamina(), PlayerStaminaComponent->GetMaxStamina());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("InGameHUD: Could not found StaminaComponent."));
		return;
	}
}