// Curios_Item.h

#pragma once

#include "CoreMinimal.h"
#include "Item/Farming_Item.h"
#include "Curios_Item.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class ECuriosType : uint8
{
	Relic		UMETA(DisplayName = "Relic"),
	Artifact	UMETA(DisplayName = "Artifact"),
	Antique		UMETA(DisplayName = "Antique"),
	Scroll		UMETA(DisplayName = "Scroll")
};

UCLASS()
class NECROMANCER_API ACurios_Item : public AFarming_Item
{
	GENERATED_BODY()

public:
	ACurios_Item();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Curios")
	UStaticMeshComponent* CuriosMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Curios")
	ECuriosType CuriosType = ECuriosType::Relic;

public:
	ECuriosType GetCuriosType() const { return CuriosType; }
};
