//weapon_Item_Bass.h

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipable_Item.h"
#include "weapon_Item_Bass.generated.h"

class UBoxComponent;

UCLASS()
class NECROMANCER_API Aweapon_Item_Bass : public AEquipable_Item
{
	GENERATED_BODY()
	
public:
	Aweapon_Item_Bass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float AttackPower;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float AttackSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitBox")
	UBoxComponent* HitBox;
};
