// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterBase.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "MonsterStatComponent.h"
#include "Necromancer.h"
#include "Net/UnrealNetwork.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AMonsterBase::AMonsterBase()
{
	MonsterStatComponent = CreateDefaultSubobject<UMonsterStatComponent>(TEXT("MonsterStatComponent"));
	SetRVOAvoidanceEnabled(true);
	
	bReplicates = true;
	SetReplicatingMovement(true);  // 이동 자동 복제
	NetUpdateFrequency = 10.0f;   // 초당 10회 업데이트 (몬스터는 플레이어보다 낮아도 됨)
	MinNetUpdateFrequency = 2.0f;
	
}


void AMonsterBase::SetRVOAvoidanceEnabled(bool bEnable)
{
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	if (MovementComponent)
	{
		MovementComponent ->bUseRVOAvoidance = bEnable;
		MovementComponent -> AvoidanceConsiderationRadius = AvoidanceRadius;
		MovementComponent -> AvoidanceWeight = AvoidanceWeight;
	}
}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	
	MonsterStatComponent->OnDeath.AddDynamic(this, &AMonsterBase::OnDeath);
	MonsterStatComponent->OnStagger.AddUObject(this, &AMonsterBase::OnStagger);
	MonsterStatComponent->OnStun.AddUObject(this, &AMonsterBase::OnStun);
}

void AMonsterBase::OnStagger()
{
	StopAnimMontage();
}

void AMonsterBase::OnStun()
{
	StopAnimMontage();
}

void AMonsterBase::OnDeath()
{
	// ★ 서버에서만 실행 ★
	if (!HasAuthority()) return;

	bIsDead = true;  // Replicated → 클라이언트에 OnRep_IsDead 호출됨

	AAIController* AIController = GetController<AAIController>();
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->StopLogic("Dead");
	}

	GetCharacterMovement()->DisableMovement();

	// 모든 클라이언트에 죽음 몽타주 전파
	Multicast_PlayDeathMontage();
}

void AMonsterBase::StartRagdoll()
{
	GetMesh()->SetAllBodiesSimulatePhysics(true);
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

FGenericTeamId AMonsterBase::GetGenericTeamId() const
{
	return FGenericTeamId(TEAM_ID_MONSTER);
}

void AMonsterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMonsterBase, bIsDead);
}

void AMonsterBase::OnRep_IsDead()
{
	if (bIsDead)
	{
		// 클라이언트에서도 이동 비활성화
		GetCharacterMovement()->DisableMovement();
	}
}

void AMonsterBase::Multicast_PlayDeathMontage_Implementation()
{
	if (DeathMontage)
	{
		float Duration = PlayAnimMontage(DeathMontage);

		FTimerHandle DeathTimerHandle;
		GetWorldTimerManager().SetTimer(DeathTimerHandle, this, &AMonsterBase::StartRagdoll, Duration, false);
	}
	else
	{
		StartRagdoll();
	}
}

void AMonsterBase::Multicast_PlayMontage_Implementation(UAnimMontage* Montage)
{
	if (Montage)
	{
		PlayAnimMontage(Montage);
	}
}