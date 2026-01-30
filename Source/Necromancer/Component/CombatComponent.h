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

public:		
	void Attack();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void PerformAttackTrace();

protected:
	UFUNCTION(Server, Reliable)
	void Server_Attack();

	UPROPERTY()
	TObjectPtr<ANecPlayerCharacter> OwnerCharacter;

	UPROPERTY()
	TObjectPtr<UStaminaComponent> OwnerStaminaComponent;

protected:
	UPROPERTY(EditAnywhere, Category = "Combat")
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackDamage = 20.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackStaminaCost = 15.0f;

	UPROPERTY(EditAnywhere, Category = "Combat")
	float AttackRange = 150.0f;

	bool bHasAppliedDamageInCurrentAttack = false;
};