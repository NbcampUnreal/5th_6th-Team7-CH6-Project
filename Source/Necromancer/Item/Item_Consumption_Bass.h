//Item_Consumption_Bass.h

#pragma once

#include "CoreMinimal.h"
#include "Item/ItemBass.h"
#include "Item_Consumption_Bass.generated.h"

class UStaticMeshComponent;
class USoundBase;

UCLASS(Abstract)
class NECROMANCER_API AItem_Consumption_Bass : public AItemBass
{
    GENERATED_BODY()

public:
    AItem_Consumption_Bass();

protected:

    virtual void ExecuteUse(class ACharacter* User) override;

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayUseEffects();

protected:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Consumable")
    UStaticMeshComponent* ItemMesh;

    UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Consumable")
    USoundBase* UseSound;
};
