#pragma once

#include "CoreMinimal.h"
#include "Item/Item_Consumption_Base/Item_Consumption_Base.h"
#include "Throwing_Item_Base.generated.h"

class ACharacter;
class AActor;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NECROMANCER_API UThrowing_Item_Base : public UItem_Consumption_Base
{
    GENERATED_BODY()

protected:
    virtual void ExecuteUseAction(ACharacter* User);

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Throwing")
    TSubclassOf<AActor> ProjectileClass;

    UPROPERTY(EditDefaultsOnly, Category = "Throwing")
    float ThrowForce = 1500.f;
};