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
};
