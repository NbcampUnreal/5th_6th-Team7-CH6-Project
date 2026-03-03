// Armor_Item_Bass.h

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBass.h"
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

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Armor")
	USkeletalMeshComponent* ArmorMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Armor|Sound")
	USoundBase* ArmorSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Replicated, Category = "Armor|Defense")
	float Defense;

};
