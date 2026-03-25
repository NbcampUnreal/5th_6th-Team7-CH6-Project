#include "AN_BossAOEDamage.h"
#include "MonsterStatComponent.h"
#include "DamageType/NecDamageType.h"
#include "Kismet/GameplayStatics.h"
#include "GenericTeamAgentInterface.h"
#include "NiagaraFunctionLibrary.h"
#include "Necromancer.h"

void UAN_BossAOEDamage::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
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

	// AOE 중심: 소켓이 있으면 소켓 위치, 없으면 ActorLocation
	FVector CenterLocation = (!AOESocketName.IsNone() && MeshComp->DoesSocketExist(AOESocketName))
		? MeshComp->GetSocketLocation(AOESocketName)
		: OwnerActor->GetActorLocation();

	// 원형 범위 판정
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);

	OwnerActor->GetWorld()->SweepMultiByChannel(
		HitResults,
		CenterLocation,
		CenterLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(AOERadius),
		Params
	);

	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (!HitActor)
		{
			continue;
		}

		// 같은 팀(몬스터) 스킵
		IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(HitActor);
		if (TeamAgent && TeamAgent->GetGenericTeamId() == FGenericTeamId(TEAM_ID_MONSTER))
		{
			continue;
		}

		// Server Only 데미지 적용
		if (OwnerActor->HasAuthority())
		{
			UGameplayStatics::ApplyDamage(
				HitActor,
				MonsterStatComponent->GetAttackPower(),
				OwnerActor->GetInstigatorController(),
				OwnerActor,
				UNecDamageType::StaticClass()
			);
		}

		// 히트 이펙트 (적중한 위치에 재생)
		FVector HitLocation = Hit.ImpactPoint;

		if (HitSound)
		{
			UGameplayStatics::PlaySoundAtLocation(OwnerActor, HitSound, HitLocation);
		}

		if (HitParticle)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				OwnerActor, HitParticle, HitLocation, FRotator::ZeroRotator, HitParticleScale);
		}
		else if (HitParticle_Cascade)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				OwnerActor->GetWorld(), HitParticle_Cascade, HitLocation, FRotator::ZeroRotator, HitParticleScale);
		}
	}
}
