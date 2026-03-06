#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HasAttackSlot.generated.h"

// BB의 HasAttackSlot 값을 체크하는 데코레이터
UCLASS()
class NECROMANCER_API UBTDecorator_HasAttackSlot : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_HasAttackSlot();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;
};
