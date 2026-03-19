#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "NecDamageType.generated.h"

UCLASS()
class NECROMANCER_API UNecDamageType : public UDamageType
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "DamageType")
	float PoiseDamage = 30.0f;
};