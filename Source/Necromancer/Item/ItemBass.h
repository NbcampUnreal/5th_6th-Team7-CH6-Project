// ItemBass.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBass.generated.h"

USTRUCT(BlueprintType)
struct FItemInstanceData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 Count = 1;
};

UCLASS(Abstract)
class NECROMANCER_API AItemBass : public AActor
{
	GENERATED_BODY()

public:
	AItemBass();

	const FItemInstanceData& GetItemData() const { return ItemData; }

	bool AddCount(int32 Amount);
	bool RemoveCount(int32 Amount);
	bool IsEmpty() const { return ItemData.Count <= 0; }

	virtual void OnDropped(const FVector& WorldLocation);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	FItemInstanceData ItemData;
};
