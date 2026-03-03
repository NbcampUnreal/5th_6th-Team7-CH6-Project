#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_DetectPlayerAttack.generated.h"

// 타겟 플레이어의 공격 상태를 감지하여 BB에 기록
UCLASS()
class NECROMANCER_API UBTService_DetectPlayerAttack : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_DetectPlayerAttack();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;
};
