#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MonsterBlock.generated.h"

// 방패 막기 상태 토글 (즉시 실행)
UCLASS()
class NECROMANCER_API UBTTask_MonsterBlock : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MonsterBlock();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual FString GetStaticDescription() const override;

protected:
	// true: 블로킹 시작, false: 블로킹 해제
	UPROPERTY(EditAnywhere, Category = "Block")
	bool bEnableBlock = true;
};
