#include "ANS_BossAOEIndicator.h"
#include "Components/DecalComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Kismet/GameplayStatics.h"
#include "Animation/AnimInstance.h"

void UANS_BossAOEIndicator::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (!MeshComp || !IndicatorMaterial)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	UWorld* World = OwnerActor->GetWorld();
	if (!World)
	{
		return;
	}

	// 기존 인디케이터가 있으면 정리
	CleanupIndicator(MeshComp);

	// AOE 중심 위치
	FVector CenterLocation = (!AOESocketName.IsNone() && MeshComp->DoesSocketExist(AOESocketName))
		? MeshComp->GetSocketLocation(AOESocketName)
		: OwnerActor->GetActorLocation();

	// 라인트레이스로 실제 바닥 위치 찾기
	FVector GroundLocation = CenterLocation;
	FRotator DecalRotation = FRotator(-90.0f, 0.0f, 0.0f);

	FHitResult FloorHit;
	FCollisionQueryParams TraceParams;
	TraceParams.AddIgnoredActor(OwnerActor);

	FVector TraceStart = CenterLocation;
	FVector TraceEnd = CenterLocation - FVector(0.0f, 0.0f, 500.0f);

	if (World->LineTraceSingleByChannel(FloorHit, TraceStart, TraceEnd, ECC_WorldStatic, TraceParams))
	{
		GroundLocation = FloorHit.ImpactPoint + FVector(0.0f, 0.0f, 1.0f);
		DecalRotation = (-FloorHit.ImpactNormal).Rotation();
	}

	// MID 생성 (색상, 진행도 파라미터 제어)
	UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(IndicatorMaterial, OwnerActor);
	MID->SetVectorParameterValue(FName("Color"), IndicatorColor);
	MID->SetScalarParameterValue(FName("Progress"), 0.0f);
	MID->SetScalarParameterValue(FName("Opacity"), IndicatorColor.A);

	// SpawnDecalAtLocation으로 데칼 생성 (엔진이 관리하는 안정적인 방식)
	float InitialRadius = 1.0f;
	UDecalComponent* Decal = UGameplayStatics::SpawnDecalAtLocation(
		OwnerActor,
		MID,
		FVector(DecalProjectionDepth, InitialRadius, InitialRadius),
		GroundLocation,
		DecalRotation,
		0.0f // LifeSpan 0 = 수동 관리
	);

	if (!Decal)
	{
		return;
	}

	// 몽타주 PlayRate 반영하여 실제 재생 시간 계산
	float PlayRate = 1.0f;
	if (UAnimInstance* AnimInstance = MeshComp->GetAnimInstance())
	{
		UAnimMontage* CurrentMontage = AnimInstance->GetCurrentActiveMontage();
		if (CurrentMontage)
		{
			PlayRate = FMath::Max(AnimInstance->Montage_GetPlayRate(CurrentMontage), 0.01f);
		}
	}

	// 활성 인디케이터 저장
	FActiveIndicator& Indicator = ActiveIndicators.Add(MeshComp);
	Indicator.Decal = Decal;
	Indicator.DynamicMaterial = MID;
	Indicator.StartWorldTime = World->GetTimeSeconds();
	Indicator.RealDuration = TotalDuration / PlayRate;  // 애니메이션 시간 → 실제 시간
	Indicator.CenterLocation = GroundLocation;
}

void UANS_BossAOEIndicator::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp || !MeshComp->GetOwner())
	{
		return;
	}

	FActiveIndicator* Indicator = ActiveIndicators.Find(MeshComp);
	if (!Indicator || !Indicator->Decal.IsValid())
	{
		return;
	}

	UWorld* World = MeshComp->GetOwner()->GetWorld();
	if (!World)
	{
		return;
	}

	// 절대 월드 시간 기반 진행도 (FrameDeltaTime 누적 대신 정확한 시간 계산)
	float Elapsed = World->GetTimeSeconds() - Indicator->StartWorldTime;
	float Progress = FMath::Clamp(Elapsed / Indicator->RealDuration, 0.0f, 3.0f);

	// 데칼 크기 업데이트 (1 → IndicatorRadius)
	float CurrentRadius = FMath::Lerp(1.0f, IndicatorRadius, Progress);
	Indicator->Decal->DecalSize = FVector(DecalProjectionDepth, CurrentRadius, CurrentRadius);
	Indicator->Decal->MarkRenderStateDirty();

	// 머티리얼 Progress 파라미터 업데이트 (머티리얼에서 펄스 등 연출 가능)
	if (Indicator->DynamicMaterial)
	{
		Indicator->DynamicMaterial->SetScalarParameterValue(FName("Progress"), Progress);
	}
}

void UANS_BossAOEIndicator::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	CleanupIndicator(MeshComp);
}

void UANS_BossAOEIndicator::CleanupIndicator(USkeletalMeshComponent* MeshComp)
{
	FActiveIndicator* Indicator = ActiveIndicators.Find(MeshComp);
	if (Indicator)
	{
		if (Indicator->Decal.IsValid())
		{
			Indicator->Decal->DestroyComponent();
		}
		ActiveIndicators.Remove(MeshComp);
	}
}
