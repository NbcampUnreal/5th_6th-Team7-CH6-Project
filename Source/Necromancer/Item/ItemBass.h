// ItemBass.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ItemBass.generated.h"

class UItemInstance;
class UItemInstanceComponent;

UCLASS(Abstract)
class NECROMANCER_API AItemBass : public AActor
{
	GENERATED_BODY()

public:
	AItemBass();

	void InitializeWithItemInstance(UItemInstance* InItemInstance);

	UFUNCTION(BlueprintCallable, Category = "Item")
	FGuid GetInstanceID() const;

	UFUNCTION(BlueprintCallable, Category = "Item")
	FName GetItemID() const;

	UFUNCTION(BlueprintCallable, Category = "Item")
	float GetDurability() const;

	UFUNCTION(BlueprintCallable, Category = "Item")
	void SetDurability(float NewDurability);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	UItemInstanceComponent* ItemInstanceComponent;
};
