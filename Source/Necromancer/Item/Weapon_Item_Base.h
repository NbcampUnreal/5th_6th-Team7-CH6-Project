//Wapeon_Itme_Bass.h

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBass.h"
#include "Weapon_Item_Base.generated.h"

class USkeletalMeshComponent;
class UBoxComponent;
class USoundBase;

USTRUCT(BlueprintType)
struct FComboActionInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName MontageSectionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StaminaCost;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DamageMultiplier = 1.0f;
};

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

	UAnimMontage* GetAttackMontage() const { return AttackMontage; }
	float GetDamage() const { return Damage; }
	float GetStaminaCost() const { return StaminaCost; }

	const TArray<FComboActionInfo>& GetComboActions() const { return ComboActions; }
	int32 GetMaxComboCount() const { return ComboActions.Num(); }
	
	void SetDamageMultiplier(float NewMultiplier) { CurrentDamageMultiplier = NewMultiplier; }

protected:
	void PerformTrace();

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
	TObjectPtr<USoundBase> AttackSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon|Sound")
	TObjectPtr<USoundBase> HitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float Damage = 10.0f;

	float CurrentDamageMultiplier = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	TObjectPtr<UAnimMontage> AttackMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Combat")
	float StaminaCost = 20.0f;

	bool bIsAttacking = false;

	TArray<AActor*> HitActors;

	FVector LastCenterLocation = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, Category = "Combat|Combo")
	TArray<FComboActionInfo> ComboActions;
};