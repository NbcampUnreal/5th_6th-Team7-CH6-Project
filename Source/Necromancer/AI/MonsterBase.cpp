// Fill out your copyright notice in the Description page of Project Settings.


#include "MonsterBase.h"

#include "AIController.h"
#include "BrainComponent.h"
#include "MonsterStatComponent.h"
#include "Component/StatComponent.h" 
#include "Necromancer.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


AMonsterBase::AMonsterBase()
{
	MonsterStatComponent = CreateDefaultSubobject<UMonsterStatComponent>(TEXT("MonsterStatComponent"));
}



void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
	
	//TO DO : 홍표님 추가
	//HealthComponent->OnDeath.AddDynamic(this, &AMonsterBase::OnDeath);

	
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
	AAIController* AIController = GetController<AAIController>();
	if (AIController && AIController->GetBrainComponent())
	{
		AIController->GetBrainComponent()->StopLogic("Dead");
	}
	
	GetCharacterMovement()->DisableMovement();
	
	float Duration = 0.0f;
	if (DeathMontage)
	{
		Duration = PlayAnimMontage(DeathMontage);
	}
	
	FTimerHandle DeathTimerHandle;
	GetWorldTimerManager().SetTimer(DeathTimerHandle,this,&AMonsterBase::StartRagdoll,Duration,false);
	
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

