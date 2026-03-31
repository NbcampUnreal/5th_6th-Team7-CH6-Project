// Armor_Item_Bass.h

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBass.h"
#include "GridInventory/NecInventoryComponent.h"
#include "Armor_Item_Bass.generated.h"

class USkeletalMeshComponent;
class USoundBase;

UCLASS(Abstract)
class NECROMANCER_API AArmor_Item_Bass : public AItemBass
{
	GENERATED_BODY()

public:
	AArmor_Item_Bass();

	float GetDefenseRate() const { return Defense; }

	virtual void Equip(AActor* Equip_Owner)override;

	void Unequip();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Armor")
	USkeletalMeshComponent* ArmorMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor|Slot")
	EEquipmentSlot ArmorSlotType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor|Sound")
	USoundBase* ArmorSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Armor|Defense")
	float Defense;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor|Stat")
	float HeadArmor = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor|Stat")
	float BodyArmor = 3.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor|Stat")
	float LegArmor = 1.0f;
};
