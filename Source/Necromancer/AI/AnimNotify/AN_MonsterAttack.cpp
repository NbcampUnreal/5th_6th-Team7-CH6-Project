// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_MonsterAttack.h"
#include "MonsterStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GenericTeamAgentInterface.h"
#include "DrawDebugHelpers.h"
#include "NiagaraFunctionLibrary.h"
#include "Necromancer.h"

void UAN_MonsterAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}
	UMonsterStatComponent* MonsterStatComponent = OwnerActor->FindComponentByClass<UMonsterStatComponent>();
	if (!MonsterStatComponent)
	{
		return;
	}
	FVector AttackStart = OwnerActor->GetActorLocation();
	FVector AttackEnd = AttackStart + OwnerActor->GetActorForwardVector() * AttackDistance;
    
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);
    
	bool bHit = OwnerActor->GetWorld()->SweepMultiByChannel(HitResults,AttackStart,AttackEnd,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeSphere(AttackRadius),Params);
	if (OwnerActor->HasAuthority())  
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
		
			if (HitActor)
			{
				IGenericTeamAgentInterface* GenericTeamAgentInterface = Cast<IGenericTeamAgentInterface>(HitActor);
				if (GenericTeamAgentInterface && GenericTeamAgentInterface -> GetGenericTeamId() == FGenericTeamId(TEAM_ID_MONSTER))
				{
					continue;
				}
				UGameplayStatics::ApplyDamage(HitActor,MonsterStatComponent->GetAttackPower(),OwnerActor->GetInstigatorController(),OwnerActor,nullptr);

				// 히트 이펙트 (사운드 + 파티클)
				FVector HitLocation = Hit.ImpactPoint;

				if (HitSound)
				{
					UGameplayStatics::PlaySoundAtLocation(OwnerActor, HitSound, HitLocation);
				}

				if (HitParticle)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(OwnerActor, HitParticle, HitLocation, FRotator::ZeroRotator, HitParticleScale);
				}
			}
		}
	}
	
}
