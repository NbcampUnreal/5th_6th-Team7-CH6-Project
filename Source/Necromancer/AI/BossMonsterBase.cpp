#include "BossMonsterBase.h"
#include "BossPhaseDataAsset.h"
#include "MonsterStatComponent.h"
#include "Necromancer.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

ABossMonsterBase::ABossMonsterBase()
{
}

void ABossMonsterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABossMonsterBase, CurrentPhase);
	DOREPLIFETIME(ABossMonsterBase, bIsTransitioning);
	DOREPLIFETIME(ABossMonsterBase, bHasSuperArmor);
	DOREPLIFETIME(ABossMonsterBase, bPatrolEnabled);
}

void ABossMonsterBase::BeginPlay()
{
	Super::BeginPlay();

	// 기본 이동 속도 저장 (페이즈 배율 기준값)
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		BaseMaxWalkSpeed = MoveComp->MaxWalkSpeed;
	}

	// 초기 페이즈 BB 키 설정
	if (AAIController* AIC = GetController<AAIController>())
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsInt(NAME_BossPhase, CurrentPhase);
		}
	}
}

void ABossMonsterBase::OnDamageReceived(float DamageAmount, FVector HitLocation, bool bPoiseBroken)
{
	if (bIsDead)
	{
		return;
	}

	// 슈퍼아머: 페이즈 전환 중 또는 특정 패턴 실행 중 (HitReact 스킵, 데미지는 받음)
	if (bIsTransitioning || bHasSuperArmor)
	{
		// Poise만 적용 (경직 판정)
		if (UMonsterStatComponent* StatComp = FindComponentByClass<UMonsterStatComponent>())
		{
			StatComp->ApplyPoise(DamageAmount);
		}
		CheckPhaseTransition();
		return;
	}

	// 기본 피격 처리 (부모 호출)
	Super::OnDamageReceived(DamageAmount, HitLocation, bPoiseBroken);

	// 페이즈 전환 체크
	CheckPhaseTransition();
}

void ABossMonsterBase::CheckPhaseTransition()
{
	if (!HasAuthority() || !PhaseData || bIsTransitioning)
	{
		return;
	}

	UMonsterStatComponent* StatComp = FindComponentByClass<UMonsterStatComponent>();
	if (!StatComp)
	{
		return;
	}

	float MaxHP = StatComp->GetMaxHealth();
	if (MaxHP <= 0.0f)
	{
		return;
	}

	float HealthRatio = StatComp->GetCurrentHealth() / MaxHP;

	// 다음 페이즈의 threshold 확인
	int32 NextPhase = CurrentPhase + 1;
	const FBossPhaseConfig* NextConfig = PhaseData->GetPhaseConfig(NextPhase);

	if (NextConfig && HealthRatio <= NextConfig->HealthThreshold)
	{
		TransitionToPhase(NextPhase);
	}
}

void ABossMonsterBase::TransitionToPhase(int32 NewPhase)
{
	if (!PhaseData)
	{
		return;
	}

	int32 OldPhase = CurrentPhase;
	CurrentPhase = NewPhase;
	bIsTransitioning = true;

	// 현재 몽타주 중지
	StopAnimMontage();

	// 스탯 변경 적용
	const FBossPhaseConfig* Config = PhaseData->GetPhaseConfig(NewPhase);
	if (Config)
	{
		// 이동 속도 배율
		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->MaxWalkSpeed = BaseMaxWalkSpeed * Config->MovementSpeedMultiplier;
		}

		// 전환 몽타주 재생
		if (Config->PhaseTransitionMontage)
		{
			Multicast_PlayMontage(Config->PhaseTransitionMontage);

			if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
			{
				FOnMontageEnded EndDelegate;
				EndDelegate.BindUObject(this, &ABossMonsterBase::OnTransitionMontageEnded);
				AnimInstance->Montage_SetEndDelegate(EndDelegate, Config->PhaseTransitionMontage);
			}
			else
			{
				bIsTransitioning = false;
			}
		}
		else
		{
			bIsTransitioning = false;
		}
	}
	else
	{
		bIsTransitioning = false;
	}

	// BB 키 업데이트
	if (AAIController* AIC = GetController<AAIController>())
	{
		if (UBlackboardComponent* BB = AIC->GetBlackboardComponent())
		{
			BB->SetValueAsInt(NAME_BossPhase, CurrentPhase);
		}
	}

	OnPhaseChanged(OldPhase, NewPhase);

	UE_LOG(LogTemp, Log, TEXT("[BossMonsterBase] Phase transition: %d -> %d"), OldPhase, NewPhase);
}

void ABossMonsterBase::OnPhaseChanged(int32 OldPhase, int32 NewPhase)
{
	// 서브클래스에서 오버라이드하여 페이즈별 특수 처리
}

void ABossMonsterBase::OnTransitionMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	bIsTransitioning = false;
	RestoreMovementIfAlive();
}

void ABossMonsterBase::OnRep_CurrentPhase()
{
	// 클라이언트에서 페이즈 변경 시 처리 (UI 업데이트 등)
}

void ABossMonsterBase::Multicast_Teleport_Implementation(FVector NewLocation, FRotator NewRotation)
{
	FVector OldLocation = GetActorLocation();
	FRotator OldRotation = GetActorRotation();

	// 사라지기 이펙트 (Niagara 우선, Cascade 폴백)
	if (TeleportDisappearEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, TeleportDisappearEffect, OldLocation, OldRotation);
	}
	else if (TeleportDisappearEffect_Cascade)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), TeleportDisappearEffect_Cascade, OldLocation, OldRotation);
	}

	if (TeleportSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, TeleportSound, OldLocation);
	}

	// 위치 이동
	SetActorLocation(NewLocation);
	SetActorRotation(NewRotation);

	// 나타나기 이펙트 (Niagara 우선, Cascade 폴백)
	if (TeleportAppearEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, TeleportAppearEffect, NewLocation, NewRotation);
	}
	else if (TeleportAppearEffect_Cascade)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), TeleportAppearEffect_Cascade, NewLocation, NewRotation);
	}
}
