#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_Strafe.generated.h"

// 타겟 주위를 좌/우로 원형 이동
UCLASS()
class NECROMANCER_API UBTTask_Strafe : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_Strafe();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

	// 스트레이프 지속 시간
	UPROPERTY(EditAnywhere, Category = "Strafe")
	float StrafeDuration = 2.0f;

	// 타겟으로부터 유지할 거리
	UPROPERTY(EditAnywhere, Category = "Strafe")
	float StrafeRadius = 400.0f;

	// 이동 속도 배율
	UPROPERTY(EditAnywhere, Category = "Strafe")
	float StrafeSpeedMultiplier = 0.6f;

private:
	float ElapsedTime = 0.0f;
	float StrafeDirection = 1.0f;
};
