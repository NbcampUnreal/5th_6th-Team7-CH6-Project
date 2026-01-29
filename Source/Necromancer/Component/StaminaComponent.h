#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStaminaChangedSignature, float, CurrentStamina, float, MaxStamina);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NECROMANCER_API UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UStaminaComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void OnRep_CurrentStamina();

	void SetStamina(float NewStamina);

public:
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool ConsumeStamina(float Amount);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StartStaminaDrain(float DrainRate);

	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void StopStaminaDrain(bool bResetExhaustion = true);

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetCurrentStamina() const { return CurrentStamina; }

	UFUNCTION(BlueprintPure, Category = "Stamina")
	float GetMaxStamina() const { return MaxStamina; }

	bool IsExhausted() const { return bIsExhausted; }
	void SetExhausted(bool bValue) { bIsExhausted = bValue; }

	UPROPERTY(BlueprintAssignable)
	FOnStaminaChangedSignature OnStaminaChanged;

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentStamina, VisibleAnywhere, Category = "Stamina")
	float CurrentStamina;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina", meta = (ClampMin = "1.0"))
	float MaxStamina = 100.0f;

	UPROPERTY(EditAnywhere, Category = "Stamina")
	float StaminaRecoveryRate;

	float CurrentDrainRate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float RecoveryThreshold = 20.0f;

	bool bIsExhausted = false;
};