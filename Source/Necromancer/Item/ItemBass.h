// ItemBass.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataTable.h"
#include "ItemBass.generated.h"

class UItemInstance;
struct FItemData;

UCLASS()
class NECROMANCER_API AItemBass : public AActor
{
	GENERATED_BODY()

public:
	AItemBass();

	void CopyFromItemInstance(class UItemInstance* ItemInstance);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Data")
	UDataTable* ItemDataTable;

	const FItemData* GetItemData() const;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FGuid InstanceID;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FName ItemID;

	UPROPERTY(Replicated, BlueprintReadWrite)
	float CurrentDurability;

	UPROPERTY(Replicated, BlueprintReadWrite)
	bool bRotated;

	UPROPERTY(Replicated, BlueprintReadOnly)
	FGuid OwnerItemGuid;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 SectionIndex;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 PosX;

	UPROPERTY(Replicated, BlueprintReadOnly)
	int32 PosY;
};
