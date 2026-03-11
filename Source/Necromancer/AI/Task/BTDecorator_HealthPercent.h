#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_HealthPercent.generated.h"

// HP 비율이 지정 범위(Min~Max) 안에 있으면 true
UCLASS()
class NECROMANCER_API UBTDecorator_HealthPercent : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_HealthPercent();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

	// HP 비율 하한 (0.0~1.0, 0.3 = 30%)
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MinHealthPercent = 0.0f;

	// HP 비율 상한 (0.0~1.0, 0.7 = 70%)
	UPROPERTY(EditAnywhere, Category = "Condition", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float MaxHealthPercent = 1.0f;
};
