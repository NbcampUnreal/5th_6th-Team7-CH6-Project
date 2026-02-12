// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_MonsterNextAttack.generated.h"

/**
 * 
 */
UCLASS()
class NECROMANCER_API UAN_MonsterNextAttack : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	

	// 연속공격 가능 최대 거리
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Attack")
	float MaxComboDistance = 300.0f;
	
};
