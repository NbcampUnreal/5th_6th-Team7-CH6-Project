// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AN_MonsterRangedAttack.generated.h"

class AMonsterProjectile;


UCLASS()
class NECROMANCER_API UAN_MonsterRangedAttack : public UAnimNotify
{
	GENERATED_BODY()

public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	TSubclassOf<AMonsterProjectile> ProjectileClass;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	FName MuzzleSocketName = "hand_r";

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile")
	bool bUsePrediction = true;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float PredictionAccuracy = 0.7f;
};
