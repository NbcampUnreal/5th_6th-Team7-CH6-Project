// Fill out your copyright notice in the Description page of Project Settings.

#include "ANS_MonsterAttackTrace.h"
#include "MonsterStatComponent.h"
#include "Necromancer.h"
#include "GenericTeamAgentInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "NiagaraFunctionLibrary.h"

void UANS_MonsterAttackTrace::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	HitActors.Empty();

	if (!MeshComp) return;

	FVector StartPos = MeshComp->GetSocketLocation(TraceStartSocket);
	FVector EndPos = MeshComp->GetSocketLocation(TraceEndSocket);
	LastCenterLocation = (StartPos + EndPos) * 0.5f;
}

void UANS_MonsterAttackTrace::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor) 
	{
		return;
	}

	if (!OwnerActor->HasAuthority()) 
	{
		return;
	}

	UMonsterStatComponent* StatComp = OwnerActor->FindComponentByClass<UMonsterStatComponent>();
	if (!StatComp) 
	{
		return;
	}

	FVector StartSocket = MeshComp->GetSocketLocation(TraceStartSocket);
	FVector EndSocket = MeshComp->GetSocketLocation(TraceEndSocket);
	FVector CurrentCenterLocation = (StartSocket + EndSocket) * 0.5f;

	FVector Direction = EndSocket - StartSocket;
	
	FRotator BoxRotation = UKismetMathLibrary::MakeRotFromX(Direction);
	float TraceLength = Direction.Size();
	FVector BoxHalfSize = FVector(TraceLength * 0.5f, TraceExtent.Y, TraceExtent.Z);

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerActor);

	TArray<FHitResult> HitResults;

	bool bHit = UKismetSystemLibrary::BoxTraceMulti(OwnerActor,LastCenterLocation,CurrentCenterLocation,BoxHalfSize,BoxRotation,UEngineTypes::ConvertToTraceType(ECC_Pawn),false,ActorsToIgnore,bShowDebugTrace ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,HitResults,true);

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor)
			{
				continue;
			}

			// 이미 이번 공격에서 맞은 액터 스킵
			bool bAlreadyHit = false;
			for (const TWeakObjectPtr<AActor>& Prev : HitActors)
			{
				if (Prev.Get() == HitActor)
				{
					bAlreadyHit = true;
					break;
				}
			}
			if (bAlreadyHit) 
			{
				continue;
			}

			// 같은 몬스터 스킵
			IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(HitActor);
			if (TeamAgent && TeamAgent->GetGenericTeamId() == FGenericTeamId(TEAM_ID_MONSTER))
			{
				continue;
			}

			HitActors.Add(HitActor);

			UGameplayStatics::ApplyDamage(HitActor,StatComp->GetAttackPower(),OwnerActor->GetInstigatorController(),OwnerActor,nullptr);

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

	LastCenterLocation = CurrentCenterLocation;
}

void UANS_MonsterAttackTrace::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	HitActors.Empty();
}

FString UANS_MonsterAttackTrace::GetNotifyName_Implementation() const
{
	return FString::Printf(TEXT("AttackTrace [%s → %s]"), *TraceStartSocket.ToString(), *TraceEndSocket.ToString());
}
