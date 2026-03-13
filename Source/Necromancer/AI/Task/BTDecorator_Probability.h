#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_Probability.generated.h"

// 설정한 확률로 true/false 반환 (공격 패턴 분기용)
UCLASS()
class NECROMANCER_API UBTDecorator_Probability : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_Probability();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

	// 0.0~1.0 (0.3 = 30% 확률로 true)
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float Probability = 0.5f;
};
