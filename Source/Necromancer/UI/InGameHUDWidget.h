#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UStatComponent;
class UStaminaComponent;
class USoulComponent;
class UNecInventoryComponent;

UCLASS()
class NECROMANCER_API UInGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateStamina(float CurrentStamina, float MaxStamina);

	void InitHUD();
	void TryBindPlayerState();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthTickDownBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaTickDownBar;

	UPROPERTY()
	TObjectPtr<UStatComponent> PlayerStatComponent;

	UPROPERTY()
	TObjectPtr<UStaminaComponent> PlayerStaminaComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<USoulComponent> SoulComponent;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UNecInventoryComponent> NecInventoryComponent;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> GraceTimeForReviveText;

protected:
	UPROPERTY(EditAnywhere, Category = "HUD")
	float HealthTickDownBarInterpSpeed = 3.0f;

	float TargetHealthPercent = 1.0f;
	float CurrentHealthTickDownPercent = 1.0f;

	UPROPERTY(EditAnywhere, Category = "HUD")
	float StaminaTickDownBarInterpSpeed = 3.0f;

	float TargetStaminaPercent = 1.0f;
	float CurrentStaminaTickDownPercent = 1.0f;

public:
	UFUNCTION()
	void UpdateGraceTimeForReviveText(int32 GraceTimeForRevive);
};