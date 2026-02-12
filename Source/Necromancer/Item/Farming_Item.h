//Failing_Item.h

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBass.h"
#include "Farming_Item.generated.h"


UCLASS()
class NECROMANCER_API AFarming_Item : public AItemBass
{
	GENERATED_BODY()

public:
	AFarming_Item();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Farming")
	bool bStackable = true;

	UPROPERTY(EditDefaultsOnly, Category = "Farming", meta = (EditCondition = "bStackable"))
	int32 MaxStackCount = 99;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	int32 SellPrice = 1;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	bool bCanBuyFromShop = true;

	UPROPERTY(EditDefaultsOnly, Category = "Shop")
	bool bCanSellToShop = true;

public:
	bool IsStackable() const { return bStackable; }
	int32 GetMaxStack() const { return MaxStackCount; }
	int32 GetSellPrice() const { return SellPrice; }

	bool CanBuyFromShop() const { return bCanBuyFromShop; }
	bool CanSellToShop() const { return bCanSellToShop; }
};
