//Wapeon_Itme_Bass.h

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBass.h"
#include "Weapon_Item_Base.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class USoundBase;

UCLASS(Abstract)
class NECROMANCER_API AWeapon_Item_Base : public AItemBass
{
	GENERATED_BODY()

public:
	AWeapon_Item_Base();

	virtual void StartAttack();

	virtual void EndAttack();

	UAnimMontage* GetAttackMontage() const { return AttackMontage; }
	float GetDamage() const { return Damage; }
	float GetStaminaCost() const { return StaminaCost; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon")
	UBoxComponent* AttackHitBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
	USoundBase* AttackSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Damage = 10.f;

	UFUNCTION()
	virtual void OnAttackHit(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float StaminaCost = 20.0f;
};
