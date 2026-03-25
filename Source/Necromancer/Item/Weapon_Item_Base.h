//Wapeon_Itme_Bass.h

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBass.h"
#include "DataAsset/WeaponDataAsset.h"
#include "Engine/StreamableManager.h"
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

protected:
	virtual void Tick(float DeltaTime) override;
	virtual void OnConstruction(const FTransform& Transform) override;

public:
	virtual void StartAttack();
	virtual void EndAttack();

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	EWeaponType GetWeaponType() const { return WeaponData ? WeaponData->WeaponType : EWeaponType::Unarmed; }

	UFUNCTION(BlueprintPure, Category = "Weapon")
	bool GetIsUnarmed() const { return bIsUnarmed; }

	UAnimMontage* GetAttackMontage() const;
	UAnimMontage* GetRunningAttackMontage() const;
	UAnimMontage* GetGuardMontage() const;

	float GetDamage() const { return WeaponData ? WeaponData->BaseDamage * CurrentDamageMultiplier : 0.0f; }
	float GetPoiseDamage() const { return WeaponData ? WeaponData->BasePoiseDamage * CurrentPoiseDamageMultiplier : 0.0f; }
	float GetGuardRate() const { return WeaponData ? WeaponData->BaseGuardRate : 0.0f; }
	
	const TArray<FComboActionInfo>& GetComboActions() const { return WeaponData ? WeaponData->ComboActions : EmptyComboActions; }
	int32 GetMaxComboCount() const { return WeaponData ? WeaponData->ComboActions.Num() : 0; }
	
	void SetDamageMultiplier(float NewMultiplier) { CurrentDamageMultiplier = NewMultiplier; }
	void SetPoiseDamageMultiplier(float NewMultiplier) { CurrentPoiseDamageMultiplier = NewMultiplier; }

	void PreloadWeaponAssets();

	virtual void Equip(AActor* Equip_Owner)override;

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitSound(FVector HitLocation);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UWeaponDataAsset> WeaponData;

	float CurrentDamageMultiplier = 1.0f;

protected:
	/*UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayHitSound(FVector HitLocation);*/

	void PerformTrace();

	void OnWeaponAssetsLoaded();

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

	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TObjectPtr<UWeaponDataAsset> WeaponData;*/

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Combat")
	bool bDrawDebug = false;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Combat")
	bool bIsUnarmed;

	/*float CurrentDamageMultiplier = 1.0f;*/

	float CurrentPoiseDamageMultiplier = 1.0f;

	bool bIsAttacking = false;

	TArray<AActor*> HitActors;

	FVector LastCenterLocation = FVector::ZeroVector;

	TSharedPtr<FStreamableHandle> AssetLoadHandle;

private:
	TArray<FComboActionInfo> EmptyComboActions;
};