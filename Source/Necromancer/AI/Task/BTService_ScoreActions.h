#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_ScoreActions.generated.h"

// 거리+HP+쿨다운 기반 행동 스코어링 → BB에 최적 행동 기록
UCLASS()
class NECROMANCER_API UBTService_ScoreActions : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_ScoreActions();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

	// 공격 가중치
	UPROPERTY(EditAnywhere, Category = "Score")
	float AttackWeight = 1.0f;

	// 회피 가중치
	UPROPERTY(EditAnywhere, Category = "Score")
	float DodgeWeight = 0.8f;

	// 방어 가중치
	UPROPERTY(EditAnywhere, Category = "Score")
	float BlockWeight = 0.6f;

	// 스트레이프 가중치
	UPROPERTY(EditAnywhere, Category = "Score")
	float StrafeWeight = 0.4f;
};
