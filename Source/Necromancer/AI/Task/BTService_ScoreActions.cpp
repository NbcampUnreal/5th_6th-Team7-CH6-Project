#include "BTService_ScoreActions.h"
#include "AIController.h"
#include "Necromancer.h"
#include "MonsterStatComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_ScoreActions::UBTService_ScoreActions()
{
	NodeName = "Score Actions";
	Interval = 0.5f;
	RandomDeviation = 0.1f;
}

void UBTService_ScoreActions::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return;
	}

	APawn* Pawn = AIC->GetPawn();
	if (!Pawn)
	{
		return;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
	if (!Target)
	{
		return;
	}

	UMonsterStatComponent* StatComp = Pawn->FindComponentByClass<UMonsterStatComponent>();
	if (!StatComp)
	{
		return;
	}

	float Distance = FVector::Dist(Pawn->GetActorLocation(), Target->GetActorLocation());
	float AttackRange = StatComp->GetAttackRange();
	bool bHasSlot = BB->GetValueAsBool(NAME_HasAttackSlot);
	bool bPlayerAttacking = BB->GetValueAsBool(NAME_PlayerIsAttacking);
	bool bCanAttack = StatComp->CanAttack();

	// 거리 비율 (0~1, 사거리 이내일수록 높음)
	float DistanceFactor = FMath::Clamp(1.0f - (Distance / (AttackRange * 3.0f)), 0.0f, 1.0f);

	// 행동별 점수 계산
	float AttackScore = 0.0f;
	float DodgeScore = 0.0f;
	float BlockScore = 0.0f;
	float StrafeScore = 0.0f;

	// 공격: 슬롯 보유 + 사거리 내 + 쿨다운 준비
	if (bHasSlot && bCanAttack)
	{
		AttackScore = AttackWeight * DistanceFactor;
	}

	// 회피: 플레이어가 공격 중 + 가까울 때
	if (bPlayerAttacking)
	{
		DodgeScore = DodgeWeight * DistanceFactor * StatComp->GetSkillLevel();
	}

	// 방어: 플레이어가 공격 중 + 중거리
	if (bPlayerAttacking)
	{
		BlockScore = BlockWeight * (1.0f - DistanceFactor * 0.5f) * StatComp->GetSkillLevel();
	}

	// 스트레이프: 슬롯 없음 + 중거리
	if (!bHasSlot)
	{
		StrafeScore = StrafeWeight * (1.0f - DistanceFactor);
	}

	// 랜덤 노이즈 추가 (행동 다양성)
	AttackScore += FMath::FRandRange(0.0f, 0.15f);
	DodgeScore += FMath::FRandRange(0.0f, 0.15f);
	BlockScore += FMath::FRandRange(0.0f, 0.15f);
	StrafeScore += FMath::FRandRange(0.0f, 0.15f);

	// 최고 점수 행동 선택
	FName BestAction = "Strafe";
	float BestScore = StrafeScore;

	if (AttackScore > BestScore)
	{
		BestAction = "Attack";
		BestScore = AttackScore;
	}
	if (DodgeScore > BestScore)
	{
		BestAction = "Dodge";
		BestScore = DodgeScore;
	}
	if (BlockScore > BestScore)
	{
		BestAction = "Block";
		BestScore = BlockScore;
	}

	BB->SetValueAsName(NAME_BestAction, BestAction);
}

FString UBTService_ScoreActions::GetStaticDescription() const
{
	return TEXT("Score Actions (Attack/Dodge/Block/Strafe)");
}
