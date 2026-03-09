//Throwing_Dagger.h
#pragma once

#include "CoreMinimal.h"
#include "Item/Item_Consumption_Base/Throwing_Item_Base.h"
#include "Throwing_Dagger.generated.h"

UCLASS()
class NECROMANCER_API UThrowing_Dagger : public UThrowing_Item_Base
{
	GENERATED_BODY()

protected:
    virtual void ExecuteUseAction(ACharacter* User) override;

protected:
    UPROPERTY(EditDefaultsOnly, Category = "Dagger")
    float Damage = 25.f;
};
