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

	UFUNCTION()
	void SetCurrentWeapon(AActor* NewWeaponActor);

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void EnableWeaponCollision();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void DisableWeaponCollision();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void OpenComboWindow();

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void CloseComboWindow();

	AWeapon_Item_Base* GetCurrentWeapon() const { return CurrentWeapon; }

	void CheckComboTransition();

	void SetGuard(bool bInGuarding);

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsGuarding() const { return bIsGuarding; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetCombatState();

protected:	
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnRep_bIsGuarding();
		
	void UpdateGuardVisuals();

	UFUNCTION(Server, Reliable)
	void Server_Attack(int32 ComboIndex);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Attack(int32 ComboIndex);

	UFUNCTION(Server, Reliable)
	void Server_SetGuard(bool bInGuarding);

	void PlayComboAttack();

protected:
	UPROPERTY()
	TObjectPtr<ANecPlayerCharacter> OwnerCharacter;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AWeapon_Item_Base> CurrentWeapon;

	UPROPERTY(ReplicatedUsing = OnRep_bIsGuarding, BlueprintReadOnly, Category = "Combat")
	bool bIsGuarding = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bIsAttacking = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bSaveAttackInput = false;

	UPROPERTY(BlueprintReadOnly, Category = "Combat")
	bool bComboWindowOpen = false;

	int32 CurrentComboIndex = 0;

	bool bHasInputBuffer = false;
};