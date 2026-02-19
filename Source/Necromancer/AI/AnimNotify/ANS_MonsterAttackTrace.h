// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "NiagaraSystem.h"
#include "ANS_MonsterAttackTrace.generated.h"

UCLASS(meta = (DisplayName = "Monster Attack Trace"))
class NECROMANCER_API UANS_MonsterAttackTrace : public UAnimNotifyState
{
	GENERATED_BODY()

public:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override;

protected:
	// 트레이스 시작 소켓 (무기 끝 또는 손/발)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FName TraceStartSocket = FName("TraceStart");

	// 트레이스 끝 소켓
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FName TraceEndSocket = FName("TraceEnd");

	// BoxTrace 반크기 (Y=좌우, Z=상하)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	FVector TraceExtent = FVector(15.0f, 15.0f, 15.0f);

	// 디버그 트레이스 표시
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trace")
	bool bShowDebugTrace = false;

	// 히트 시 재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	USoundBase* HitSound = nullptr;

	// 히트 시 스폰할 나이아가라 파티클
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	UNiagaraSystem* HitParticle = nullptr;

	// 파티클 스케일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	FVector HitParticleScale = FVector(1.0f);

private:
	FVector LastCenterLocation;
	TArray<TWeakObjectPtr<AActor>> HitActors;
};
