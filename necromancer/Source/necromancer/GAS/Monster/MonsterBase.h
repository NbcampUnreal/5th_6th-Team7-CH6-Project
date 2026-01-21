#pragma once

#include "CoreMinimal.h"
#include "GAS/Base/BaseCharacter.h"
#include "MonsterBase.generated.h"

class UMonsterAttributeSet;

UCLASS(Abstract)
class NECROMANCER_API AMonsterBase : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AMonsterBase();

	UMonsterAttributeSet* GetMonsterAttributeSet() const { return MonsterAttributeSet; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "GAS")
	TObjectPtr<UMonsterAttributeSet> MonsterAttributeSet;
};
