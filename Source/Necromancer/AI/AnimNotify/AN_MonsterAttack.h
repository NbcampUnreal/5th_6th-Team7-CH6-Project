// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "NiagaraSystem.h"
#include "AN_MonsterAttack.generated.h"

/**
 * 
 */
UCLASS()
class NECROMANCER_API UAN_MonsterAttack : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;
	
	

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackRadius = 100.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float AttackDistance = 150.0f;

	// 히트 시 재생할 사운드
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	USoundBase* HitSound = nullptr;

	// 히트 시 스폰할 나이아가라 파티클
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	UNiagaraSystem* HitParticle = nullptr;

	// 파티클 스케일
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Hit Effect")
	FVector HitParticleScale = FVector(1.0f);
};
