//Wapeon_Itme_Bass.h

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBass.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Weapon_Item_Base.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class USoundBase;
class UNiagaraSystem;

UCLASS(Abstract)
class NECROMANCER_API AWeapon_Item_Base : public AItemBass
{
	GENERATED_BODY()

public:
	AWeapon_Item_Base();

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void StartAttack();
	virtual void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EWeaponType GetWeaponType() const { return WeaponData ? WeaponData->WeaponType : EWeaponType::Unarmed; }

	UAnimMontage* GetAttackMontage() const { return WeaponData ? WeaponData->AttackMontage : nullptr; }
	UAnimMontage* GetGuardMontage() const { return WeaponData ? WeaponData->GuardMontage : nullptr; }

	float GetDamage() const { return WeaponData ? WeaponData->BaseDamage : 0.0f; }
	float GetGuardRate() const { return WeaponData ? WeaponData->BaseGuardRate : 0.0f; }
	
	const TArray<FComboActionInfo>& GetComboActions() const { return WeaponData ? WeaponData->ComboActions : EmptyComboActions; }
	int32 GetMaxComboCount() const { return WeaponData ? WeaponData->ComboActions.Num() : 0; }
	
	void SetDamageMultiplier(float NewMultiplier) { CurrentDamageMultiplier = NewMultiplier; }

protected:
	void PerformTrace();

	void PlayHitEffect(const FVector& Location);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<USkeletalMeshComponent> WeaponMesh;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Combat")
	FVector TraceExtent = FVector(30.0f, 30.0f, 30.0f);

	UPROPERTY(VisibleAnywhere, Category = "Weapon|Combat")
	TObjectPtr<UBoxComponent> TracePreviewBox;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Combat")
	FName StartSocketName = FName("TraceStart");

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Combat")
	FName EndSocketName = FName("TraceEnd");

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UWeaponDataAsset> WeaponData;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Combat")
	bool bDrawDebug = false;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|FX")
	TObjectPtr<UNiagaraSystem> HitEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|FX")
	TObjectPtr<USoundBase> HitSound;

	float CurrentDamageMultiplier = 1.0f;

	bool bIsAttacking = false;

	TArray<AActor*> HitActors;

	FVector LastCenterLocation = FVector::ZeroVector;

private:
	TArray<FComboActionInfo> EmptyComboActions;
};