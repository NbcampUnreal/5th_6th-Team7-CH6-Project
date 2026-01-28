// Equipable_Item.h

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBass.h"
#include "Equipable_Item.generated.h"

class ACharacter;

UCLASS(Abstract)
class NECROMANCER_API AEquipable_Item : public AItemBass
{
	GENERATED_BODY()

public:
	AEquipable_Item();

	virtual void OnEquip(ACharacter* OwnerCharacter);
	virtual void OnUnequip();

	bool IsEquipped() const { return bEquipped; }

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Equip")
	FName AttachSocketName = NAME_None;

	UPROPERTY(Replicated)
	bool bEquipped = false;

	UPROPERTY()
	TWeakObjectPtr<ACharacter> EquippedCharacter;

protected:
	// 실제 장착 처리
	virtual void AttachToCharacter(ACharacter* OwnerCharacter);
	virtual void DetachFromCharacter();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
