#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ANecPlayerCharacter;
class UStaminaComponent;
class AWeapon_Item_Base;

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

	void EquipWeapon(AWeapon_Item_Base* NewWeapon);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EnableWeaponCollision();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DisableWeaponCollision();

	AWeapon_Item_Base* GetCurrentWeapon() const { return CurrentWeapon; }

	void SetGuard(bool bInGuarding);

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsGuarding() const { return bIsGuarding; }

protected:	
	UFUNCTION()
	void OnRep_bIsGuarding();
		
	void UpdateGuardVisuals();

	UFUNCTION(Server, Reliable)
	void Server_Attack();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Attack();

	UFUNCTION(Server, Reliable)
	void Server_SetGuard(bool bInGuarding);

protected:
	UPROPERTY()
	TObjectPtr<ANecPlayerCharacter> OwnerCharacter;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AWeapon_Item_Base> CurrentWeapon;

	UPROPERTY(EditAnywhere, Category = "Combat")
	TObjectPtr<UAnimMontage> GuardMontage;

	UPROPERTY(ReplicatedUsing = OnRep_bIsGuarding, BlueprintReadOnly, Category = "Combat")
	bool bIsGuarding = false;
};