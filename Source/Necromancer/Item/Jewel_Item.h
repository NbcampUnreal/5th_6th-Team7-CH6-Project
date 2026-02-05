// Jewel_Item.h

#pragma once

#include "CoreMinimal.h"
#include "Item/Farming_Item.h"
#include "Jewel_Item.generated.h"

class UStaticMeshComponent;

UENUM(BlueprintType)
enum class EJewelType : uint8
{
	Ruby		UMETA(DisplayName = "Ruby"),
	Sapphire	UMETA(DisplayName = "Sapphire"),
	Emerald		UMETA(DisplayName = "Emerald"),
	Diamond		UMETA(DisplayName = "Diamond")
};

UCLASS()
class NECROMANCER_API AJewel_Item : public AFarming_Item
{
	GENERATED_BODY()

public:
	AJewel_Item();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Jewel")
	UStaticMeshComponent* JewelMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Jewel")
	EJewelType JewelType = EJewelType::Ruby;

public:
	EJewelType GetJewelType() const { return JewelType; }
};
