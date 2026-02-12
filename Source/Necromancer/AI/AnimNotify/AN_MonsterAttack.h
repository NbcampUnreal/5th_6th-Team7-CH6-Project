// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
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

};
