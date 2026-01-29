#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;
class UStatComponent;
class UStaminaComponent;

UCLASS()
class NECROMANCER_API UInGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

public:
	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateStamina(float CurrentStamina, float MaxStamina);

	void InitHUD();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> StaminaBar;

	UPROPERTY()
	TObjectPtr<UStatComponent> PlayerStatComponent;

	UPROPERTY()
	TObjectPtr<UStaminaComponent> PlayerStaminaComponent;
};