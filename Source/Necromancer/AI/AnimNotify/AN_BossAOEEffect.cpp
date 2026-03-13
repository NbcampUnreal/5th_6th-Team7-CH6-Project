#include "AN_BossAOEEffect.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

void UAN_BossAOEEffect::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	// AOE 중심: 소켓이 있으면 소켓 위치, 없으면 ActorLocation
	FVector CenterLocation = (!AOESocketName.IsNone() && MeshComp->DoesSocketExist(AOESocketName))
		? MeshComp->GetSocketLocation(AOESocketName)
		: OwnerActor->GetActorLocation();

	// AOE 바닥 이펙트 (Niagara 우선, Cascade 폴백)
	if (AOEEffect)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			OwnerActor, AOEEffect, CenterLocation, FRotator::ZeroRotator, AOEEffectScale);
	}
	else if (AOEEffect_Cascade)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			OwnerActor->GetWorld(), AOEEffect_Cascade, CenterLocation, FRotator::ZeroRotator, AOEEffectScale);
	}

	if (AOESound)
	{
		UGameplayStatics::PlaySoundAtLocation(OwnerActor, AOESound, CenterLocation);
	}
}
