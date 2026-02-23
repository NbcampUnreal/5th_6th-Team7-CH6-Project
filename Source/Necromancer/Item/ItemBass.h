// ItemBass.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "GridInventory/ItemInstance/ItemInstanceComponent.h"
#include "ItemBass.generated.h"

USTRUCT(BlueprintType)
struct FItemInstanceData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ItemID;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 CurrentDurability = 0;
};

UCLASS()
class NECROMANCER_API AItemBass : public AActor
{
    GENERATED_BODY()

public:
    AItemBass();

    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

    const FItemInstanceData& GetItemData() const { return ItemData; }

    UFUNCTION(Server, Reliable)
    void Server_Use(class ACharacter* User);

    UFUNCTION(Server, Reliable)
    void Server_Drop(const FVector& DropLocation);

    bool ReduceDurability(int32 Amount);
    bool IsBroken() const { return ItemData.CurrentDurability <= 0; }

protected:
    virtual void ExecuteUse(class ACharacter* User);

protected:
    UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    FItemInstanceData ItemData;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
    UItemInstanceComponent* ItemInstanceComponent;
};