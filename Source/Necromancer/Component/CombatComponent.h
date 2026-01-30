#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ANecPlayerCharacter;
class UStaminaComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NECROMANCER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:		
	UCombatComponent();

protected:	
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:		
	void Attack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformAttackTrace();

	void SetGuard(bool bInGuarding);

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsGuarding() const { return bIsGuarding; }

protected:	
	UFUNCTION()
	void OnRep_bIsGuarding();
		
	void UpdateGuardVisuals();

	UFUNCTION(Server, Reliable)
	void Server_Attack();

	UFUNCTION(Server, Reliable)
	void Server_SetGuard(bool bInGuarding);

	UPROPERTY()
	TObjectPtr<ANecPlayerCharacter> OwnerCharacter;

protected:
	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> GuardMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackDamage = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackStaminaCost = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 150.0f;

	bool bHasAppliedDamageInCurrentAttack = false;

	UPROPERTY(ReplicatedUsing = OnRep_bIsGuarding, BlueprintReadOnly, Category = "Combat")
	bool bIsGuarding = false;
};