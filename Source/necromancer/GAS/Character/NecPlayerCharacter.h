#pragma once

#include "CoreMinimal.h"
#include "GAS/Base/BaseCharacter.h"
#include "GameplayEffectTypes.h"
#include "NecPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
class UCharacterAttributeSet;
class UGameplayEffect;
struct FInputActionValue;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAttributeChangedDelegate, float, CurrentValue, float, MaxValue);

UCLASS()
class NECROMANCER_API ANecPlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	ANecPlayerCharacter();
		
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UPROPERTY(BlueprintAssignable, Category = "Combat|UI")
	FOnAttributeChangedDelegate OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category = "Combat|UI")
	FOnAttributeChangedDelegate OnStaminaChanged;

protected:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	void InitAbilityActorInfo();

protected:	
	void Move(const FInputActionValue& Value);		
	void Look(const FInputActionValue& Value);
	void Attack(const FInputActionValue& Value);

public:	
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpEnd();

	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }	

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* AttackAction;

protected:
	UPROPERTY()
	UCharacterAttributeSet* AttributeSet;

	UPROPERTY(EditAnywhere, Category = "GAS")
	TArray<TSubclassOf<UGameplayAbility>> DefaultAbilities;

	void OnHealthChangedCallback(const FOnAttributeChangeData& Data) const;
	void OnStaminaChangedCallback(const FOnAttributeChangeData& Data) const;

	virtual void NotifyControllerChanged() override;	

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	FGameplayTag AttackAbilityTag;

	UPROPERTY(EditDefaultsOnly, Category = "GAS")
	FGameplayTag ComboInputTag;

protected:
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* TestPoisonAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* TestHealingAction;

	UPROPERTY(EditAnywhere, Category = "Effect|Test")
	TSubclassOf<UGameplayEffect> PoisonEffectClass;

	UPROPERTY(EditAnywhere, Category = "Effect|Test")
	TSubclassOf<UGameplayEffect> HealingEffectClass;

	UPROPERTY(EditAnywhere, Category = "Effect|Test")
	TSubclassOf<UGameplayEffect> RegainStaminaEffectClass;

	void ApplyPoisonDebuff();
	void ApplyHealingBuff();
};