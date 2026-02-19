// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MonsterAIController.generated.h"

class UAIPerceptionComponent;
class UBehaviorTree;

UCLASS()
class NECROMANCER_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterAIController();

	// 타겟 설정 (BB키 연동)
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetTargetActor(AActor* NewTarget);

	UFUNCTION(BlueprintPure, Category = "AI")
	AActor* GetTargetActor() const;

	// 타겟 해제 (BB키 초기화)
	UFUNCTION(BlueprintCallable, Category = "AI")
	void ClearTargetActor();

	// 타겟 마지막 위치 저장
	UFUNCTION(BlueprintCallable, Category = "AI")
	void SetlastLocation(FVector LastLocation);

	UFUNCTION(BlueprintCallable, Category = "AI")
	FVector GetlastLocation() const;

protected:
	virtual void BeginPlay() override;
	
	FTimerHandle LoseTargetTimerHandle;

	// 타겟 감지 해제 후 대기 시간
	UPROPERTY(EditDefaultsOnly)
	float ClearTime = 3.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	
	
	// 감지 콜백
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

};