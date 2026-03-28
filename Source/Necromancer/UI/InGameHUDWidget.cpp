#include "UI/InGameHUDWidget.h"
#include "Game/NecPlayerState.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Component/StatComponent.h"
#include "Component/StaminaComponent.h"
#include "GridInventory/GridInventoryComponent.h"

#include "Character/NecPlayerCharacter.h"
#include "Game/NecPlayerState.h"

void UInGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InitHUD();

	TryBindPlayerState();
}

void UInGameHUDWidget::TryBindPlayerState()
{
	APawn* OwningPawn = GetOwningPlayerPawn();
	ANecPlayerState* NecPS = OwningPawn ? OwningPawn->GetPlayerState<ANecPlayerState>() : nullptr;

	if (NecPS)
	{
		NecPS->OnGraceTimeChanged.AddDynamic(this, &UInGameHUDWidget::UpdateGraceTimeForReviveText);
		UpdateGraceTimeForReviveText(NecPS->GraceTimeForRevive);
	}
	else
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &UInGameHUDWidget::TryBindPlayerState, 0.1f, false);
	}
}

void UInGameHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (HealthTickDownBar && CurrentHealthTickDownPercent > TargetHealthPercent)
	{
		CurrentHealthTickDownPercent = FMath::FInterpTo(CurrentHealthTickDownPercent, TargetHealthPercent, InDeltaTime, HealthTickDownBarInterpSpeed);
		HealthTickDownBar->SetPercent(CurrentHealthTickDownPercent);
	}

	if (StaminaTickDownBar && CurrentStaminaTickDownPercent > TargetStaminaPercent)
	{
		CurrentStaminaTickDownPercent = FMath::FInterpTo(CurrentStaminaTickDownPercent, TargetStaminaPercent, InDeltaTime, StaminaTickDownBarInterpSpeed);
		StaminaTickDownBar->SetPercent(CurrentStaminaTickDownPercent);
	}
}

void UInGameHUDWidget::UpdateHealth(float CurrentHealth, float MaxHealth)
{
	if (HealthBar && MaxHealth > 0.0f)
	{
		TargetHealthPercent = FMath::Clamp(CurrentHealth / MaxHealth, 0.0f, 1.0f);
		HealthBar->SetPercent(TargetHealthPercent);

		if (HealthTickDownBar && CurrentHealthTickDownPercent < TargetHealthPercent)
		{
			CurrentHealthTickDownPercent = TargetHealthPercent;
			HealthTickDownBar->SetPercent(CurrentHealthTickDownPercent);
		}
	}
}

void UInGameHUDWidget::UpdateStamina(float CurrentStamina, float MaxStamina)
{
	if (StaminaBar && MaxStamina > 0.0f)
	{
		TargetStaminaPercent = FMath::Clamp(CurrentStamina / MaxStamina, 0.0f, 1.0f);
		StaminaBar->SetPercent(TargetStaminaPercent);

		if (StaminaTickDownBar && CurrentStaminaTickDownPercent < TargetStaminaPercent)
		{
			CurrentStaminaTickDownPercent = TargetStaminaPercent;
			StaminaTickDownBar->SetPercent(CurrentStaminaTickDownPercent);
		}
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

void UInGameHUDWidget::UpdateGraceTimeForReviveText(int32 GraceTimeForRevive)
{
	if (GraceTimeForReviveText == nullptr) {
		return;
	}
	
	if (GraceTimeForRevive > 0)
	{
		GraceTimeForReviveText->SetVisibility(ESlateVisibility::Visible);

		FString CombinedString = FString::Printf(TEXT("부활가능까지 남은시간 %d"), GraceTimeForRevive);
		GraceTimeForReviveText->SetText(FText::FromString(CombinedString));
	}
	else
	{
		GraceTimeForReviveText->SetVisibility(ESlateVisibility::Hidden);
	}
}
