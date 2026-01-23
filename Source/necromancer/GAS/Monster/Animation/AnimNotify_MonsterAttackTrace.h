// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

  #include "CoreMinimal.h"
  #include "Animation/AnimNotifies/AnimNotify.h"
  #include "AnimNotify_MonsterAttackTrace.generated.h"

  UCLASS()
class NECROMANCER_API UAnimNotify_MonsterAttackTrace : public UAnimNotify
  {
  	GENERATED_BODY()

  public:
  	UAnimNotify_MonsterAttackTrace();

  	virtual void Notify(USkeletalMeshComponent* MeshComp,
			  UAnimSequenceBase* Animation,
			  const FAnimNotifyEventReference& EventReference) override;

  	virtual FString GetNotifyName_Implementation() const override;

  protected:
  	UPROPERTY(EditAnywhere, Category = "Attack")
  	FName AttackBoneName = TEXT("weapon_r");
  };