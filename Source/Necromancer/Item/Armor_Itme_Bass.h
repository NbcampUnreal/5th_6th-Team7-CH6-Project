//Armor_Itme_Bass.h

#pragma once

#include "CoreMinimal.h"
#include "Item/Equipable_Item.h"
#include "Armor_Itme_Bass.generated.h"

UCLASS()
class NECROMANCER_API AArmor_Itme_Bass : public AEquipable_Item
{
	GENERATED_BODY()
	
public:
	AArmor_Itme_Bass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float DamageReductionPercent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
	float Weight;
};
