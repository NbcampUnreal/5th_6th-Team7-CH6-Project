// Recovery_Potion.h

#pragma once

#include "CoreMinimal.h"
#include "Item/Item_Consumption_Bass.h"
#include "Recovery_Potion.generated.h"

UCLASS()
class NECROMANCER_API ARecovery_Potion : public AItem_Consumption_Bass
{
	GENERATED_BODY()

public:
	ARecovery_Potion();

protected:
	virtual void ExecuteUse(class ACharacter* User) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Recovery")
	float HealAmount = 50.f;
};


