// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "Perception/AISenseConfig_Hearing.h"
#include "MonsterAIController.generated.h"

class UAIPerceptionComponent;
class UAISenseConfig_Hearing;
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
	
	FTimerHandle AggroResetTimerHandle;

	// 어그로 전파 재사용 대기 시간 (타겟 해제 후)
	UPROPERTY(EditDefaultsOnly, Category = "AI|Aggro")
	float AggroResetTime = 30.0f;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;

	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UAISenseConfig_Hearing> HearingConfig;

	// Hearing 감지 범위 (어그로 전파 수신 범위)
	UPROPERTY(EditDefaultsOnly, Category = "AI|Hearing")
	float HearingRange = 1500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	
	
	// 감지 콜백
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	// 어그로 전파 BB키 리셋
	void ResetAggroPropagation();
};