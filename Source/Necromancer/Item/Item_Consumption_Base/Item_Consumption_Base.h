#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "GridInventory/ItemData/ItemData.h"
#include "Item_Consumption_Base.generated.h"

class USoundBase;
class ACharacter;
class UItemInstance;

UCLASS(Blueprintable, EditInlineNew, DefaultToInstanced)
class NECROMANCER_API UItem_Consumption_Base : public UObject
{
    GENERATED_BODY()

public:
    void Initialize(const FItemData& InItemData, UItemInstance* InOwnerItem);

    virtual void Use(ACharacter* User);

    bool IsBroken() const;

    const FItemData& GetItemData() const { return ItemData; }

protected:
    void DecreaseDurability();

    void ExecuteUseAction(ACharacter* User);

    void SyncToInventory();

protected:
    FItemData ItemData;

    int32 CurrentDurability;

    UPROPERTY()
    UItemInstance* OwnerItem;
};