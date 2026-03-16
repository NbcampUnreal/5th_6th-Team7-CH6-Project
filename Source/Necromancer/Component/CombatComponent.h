#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DataAsset/WeaponDataAsset.h"
#include "CombatComponent.generated.h"

class ANecPlayerCharacter;
class UStaminaComponent;
class AWeapon_Item_Base;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnWeaponChangedSignature, EWeaponType, NewWeaponType);

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

	UFUNCTION(BlueprintPure, Category = "Combat")
	bool IsAttacking() const { return bIsAttacking; }

	UFUNCTION(BlueprintCallable, Category = "Combat")
	void ResetCombatState();

	void UpdateGuardVisuals();

	UPROPERTY(BlueprintAssignable)
	FOnWeaponChangedSignature OnWeaponChanged;

protected:	
	UFUNCTION()
	void OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted);

	UFUNCTION()
	void OnRep_bIsGuarding();

	UFUNCTION()
	void OnRep_CurrentWeapon();

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

	UPROPERTY(ReplicatedUsing = OnRep_CurrentWeapon, VisibleAnywhere, BlueprintReadOnly, Category = "Combat")
	TObjectPtr<AWeapon_Item_Base> CurrentWeapon;

	UPROPERTY()
	TObjectPtr<UAnimMontage> ActiveAttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TSubclassOf<AWeapon_Item_Base> UnarmedWeaponClass;

	UPROPERTY(Replicated)
	TObjectPtr<AWeapon_Item_Base> UnarmedWeaponInstance;

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