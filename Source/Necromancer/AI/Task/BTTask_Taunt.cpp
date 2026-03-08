#include "BTTask_Taunt.h"
#include "AIController.h"
#include "MonsterBase.h"
#include "MonsterAIController.h"
#include "Necromancer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

UBTTask_Taunt::UBTTask_Taunt()
{
	NodeName = "Taunt";
	bCreateNodeInstance = true;
	bNotifyTick = false;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_Taunt::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	// 이미 사용했으면 실행하지 않음
	if (!BB->GetValueAsBool(NAME_CanPropagateAggro))
	{
		return EBTNodeResult::Failed;
	}

	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return EBTNodeResult::Failed;
	}

	AMonsterBase* Monster = Cast<AMonsterBase>(AIC->GetPawn());
	if (!Monster || !Monster->HasAuthority())
	{
		return EBTNodeResult::Failed;
	}

	AActor* Target = Cast<AActor>(BB->GetValueAsObject(NAME_TargetActor));
	if (!Target)
	{
		return EBTNodeResult::Failed;
	}

	// 주변 몬스터에게 직접 타겟 전파
	PropagateAggroToNearby(Monster, Target);

	// 사용 완료 표시
	BB->SetValueAsBool(NAME_CanPropagateAggro, false);

	// 몽타주 없으면 즉시 완료
	if (!TauntMontage)
	{
		return EBTNodeResult::Succeeded;
	}

	// AnimInstance 확인
	USkeletalMeshComponent* Mesh = Monster->GetMesh();
	if (!Mesh)
	{
		return EBTNodeResult::Failed;
	}

	UAnimInstance* AnimInstance = Mesh->GetAnimInstance();
	if (!AnimInstance)
	{
		return EBTNodeResult::Failed;
	}

	// 이동 정지 + 몽타주 재생
	Monster->GetCharacterMovement()->StopMovementImmediately();
	Monster->Multicast_PlayMontage(TauntMontage);

	float Duration = TauntMontage->GetPlayLength();
	if (Duration <= 0.0f)
	{
		return EBTNodeResult::Failed;
	}

	bTaskActive = true;

	// 몽타주 종료 대기
	FOnMontageEnded EndDelegate;
	EndDelegate.BindUObject(this, &UBTTask_Taunt::OnMontageEnded, &OwnerComp);
	AnimInstance->Montage_SetEndDelegate(EndDelegate, TauntMontage);

	// 안전 타이머
	if (UWorld* World = Monster->GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SafetyTimerHandle,
			FTimerDelegate::CreateUObject(this, &UBTTask_Taunt::OnSafetyTimeout, &OwnerComp),
			Duration + TimeoutBuffer,
			false
		);
	}

	return EBTNodeResult::InProgress;
}

void UBTTask_Taunt::PropagateAggroToNearby(AMonsterBase* SourceMonster, AActor* Target)
{
	UWorld* World = SourceMonster->GetWorld();
	if (!World)
	{
		return;
	}

	const FVector Origin = SourceMonster->GetActorLocation();

	// 범위 내 MonsterBase 검색
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(World, AMonsterBase::StaticClass(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		AMonsterBase* OtherMonster = Cast<AMonsterBase>(Actor);
		if (!OtherMonster || OtherMonster == SourceMonster || OtherMonster->GetIsDead())
		{
			continue;
		}

		// 범위 체크
		float Distance = FVector::Dist(Origin, OtherMonster->GetActorLocation());
		if (Distance > AggroRange)
		{
			continue;
		}

		// 해당 몬스터의 AI Controller를 통해 타겟 설정
		AMonsterAIController* OtherAIC = Cast<AMonsterAIController>(OtherMonster->GetController());
		if (!OtherAIC)
		{
			continue;
		}

		// 이미 타겟이 있으면 스킵
		if (OtherAIC->GetTargetActor())
		{
			continue;
		}

		OtherAIC->SetTargetActor(Target);
	}
}

void UBTTask_Taunt::OnMontageEnded(UAnimMontage* Montage, bool bInterrupted, UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !bTaskActive)
	{
		return;
	}

	CleanupState(OwnerComp);
	FinishLatentTask(*OwnerComp, bInterrupted ? EBTNodeResult::Failed : EBTNodeResult::Succeeded);
}

void UBTTask_Taunt::OnSafetyTimeout(UBehaviorTreeComponent* OwnerComp)
{
	if (!OwnerComp || !bTaskActive)
	{
		return;
	}

	CleanupState(OwnerComp);
	FinishLatentTask(*OwnerComp, EBTNodeResult::Failed);
}

void UBTTask_Taunt::CleanupState(UBehaviorTreeComponent* OwnerComp)
{
	bTaskActive = false;

	if (AAIController* AIC = OwnerComp->GetAIOwner())
	{
		if (APawn* Pawn = AIC->GetPawn())
		{
			Pawn->GetWorld()->GetTimerManager().ClearTimer(SafetyTimerHandle);

			if (AMonsterBase* Monster = Cast<AMonsterBase>(Pawn))
			{
				Monster->RestoreMovementIfAlive();
			}
		}
	}
}

void UBTTask_Taunt::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	if (bTaskActive)
	{
		CleanupState(&OwnerComp);
	}

	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}

FString UBTTask_Taunt::GetStaticDescription() const
{
	FString MontageName = TauntMontage ? TauntMontage->GetName() : TEXT("None");
	return FString::Printf(TEXT("Taunt (Range=%.0f, Montage=%s)"), AggroRange, *MontageName);
}
