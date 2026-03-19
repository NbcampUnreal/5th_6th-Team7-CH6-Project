//Recovery_Item.h

#pragma once

#include "CoreMinimal.h"
#include "Item/Item_Consumption_Base/Item_Consumption_Base.h"
#include "Recovery_Item.generated.h"

UCLASS(BlueprintType, Blueprintable)
class NECROMANCER_API URecovery_Item : public UItem_Consumption_Base
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Item")
	virtual void Use(ACharacter* User)override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recovery")
	float RecoverAmount = 30.f;
};