#include "UI/InGameHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Component/StatComponent.h"
#include "Component/StaminaComponent.h"

void UInGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	APawn* OwningPawn = GetOwningPlayerPawn();
	if (!OwningPawn)
	{
		return;
	}
		
	PlayerStatComponent = OwningPawn->FindComponentByClass<UStatComponent>();
	if (!PlayerStatComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InGameHUD: Could not found StatComponent."));
		return;		
	}
	else
	{
		PlayerStatComponent->OnHealthChanged.AddDynamic(this, &UInGameHUDWidget::UpdateHealth);
		UpdateHealth(PlayerStatComponent->GetCurrentHealth(), PlayerStatComponent->GetMaxHealth());
	}
	
	PlayerStaminaComponent = OwningPawn->FindComponentByClass<UStaminaComponent>();
	if (!PlayerStaminaComponent)
	{
		UE_LOG(LogTemp, Warning, TEXT("InGameHUD: Could not found StaminaComponent."));
		return;
	}	
	else
	{
		PlayerStaminaComponent->OnStaminaChanged.AddDynamic(this, &UInGameHUDWidget::UpdateStamina);
		UpdateStamina(PlayerStaminaComponent->GetCurrentStamina(), PlayerStaminaComponent->GetMaxStamina());
	}
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