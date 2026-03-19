#include "Item/Item_Consumption_Base/Item_Consumption_Base.h"
#include "GridInventory/ItemInstance/ItemInstance.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void UItem_Consumption_Base::Initialize(const FItemData& InItemData, UItemInstance* InOwnerItem)
{
    ItemData = InItemData;
    CurrentDurability = InOwnerItem->CurrentDurability;
    OwnerItem = InOwnerItem;
}

bool UItem_Consumption_Base::IsBroken() const
{
    return CurrentDurability <= 0;
}

void UItem_Consumption_Base::Use(ACharacter* User)
{
    if (!User || IsBroken())
        return;

    ExecuteUseAction(User);

    DecreaseDurability();

    SyncToInventory();

    if (ItemData.UseActionClass)
    {
        if (USoundBase* Sound = Cast<USoundBase>(ItemData.UseActionClass->GetDefaultObject()))
        {
            UGameplayStatics::PlaySoundAtLocation(
                User,
                Sound,
                User->GetActorLocation()
            );
        }
    }
}

void UItem_Consumption_Base::DecreaseDurability()
{
    CurrentDurability--;

    if (CurrentDurability <= 0)
    {
        CurrentDurability = 0;
    }
}

void UItem_Consumption_Base::SyncToInventory()
{
    if (!OwnerItem)
        return;

    OwnerItem->CurrentDurability = CurrentDurability;
}

void UItem_Consumption_Base::ExecuteUseAction(ACharacter* User)
{
    if (!ItemData.UseActionClass)
        return;

    UObject* ActionObj = NewObject<UObject>(this, ItemData.UseActionClass);

    if (!ActionObj)
        return;

    UFunction* Func = ActionObj->FindFunction(TEXT("Execute"));

    if (Func)
    {
        struct FDynamicParams
        {
            ACharacter* User;
        };

        FDynamicParams Params;
        Params.User = User;

        ActionObj->ProcessEvent(Func, &Params);
    }
}