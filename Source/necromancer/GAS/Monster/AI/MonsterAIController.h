// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Perception/AIPerceptionTypes.h"
#include "MonsterAIController.generated.h"

class UStateTreeAIComponent;
class UAIPerceptionComponent;
class UAISenseConfig_Sight;

UCLASS()
class NECROMANCER_API AMonsterAIController : public AAIController
{
	GENERATED_BODY()

public:
	AMonsterAIController();

	// 타겟 관리
	UFUNCTION(BlueprintPure, Category = "AI|Target")
	AActor* GetCurrentTarget() const { return CurrentTarget; }

	UFUNCTION(BlueprintCallable, Category = "AI|Target")
	void SetCurrentTarget(AActor* NewTarget);

	UFUNCTION(BlueprintCallable, Category = "AI|Target")
	void ClearCurrentTarget();

	// 마지막 목격 위치 관련
	UFUNCTION(BlueprintPure, Category = "AI|Target")
	FVector GetLastKnownTargetLocation() const { return LastKnownTargetLocation; }

	UFUNCTION(BlueprintPure, Category = "AI|Target")
	bool HasLastKnownLocation() const { return bHasLastKnownLocation; }

	// 외부에서 호출 (Combat 태그 제거 포함)
	UFUNCTION(BlueprintCallable, Category = "AI|Target")
	void ClearLastKnownLocation();

	// MonsterBase에서 호출하는 내부용 함수
	void SetLastKnownLocation(const FVector& Location);
	void ClearLastKnownLocationInternal();

	

protected:
	// 스테이트 트리 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Components")
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent;


	// 현재 추적중인 타겟
	UPROPERTY(BlueprintReadOnly, Category = "AI|Target")
	TObjectPtr<AActor> CurrentTarget;

	// 타겟의 마지막 목격 위치
	UPROPERTY(BlueprintReadOnly, Category = "AI|Target")
	FVector LastKnownTargetLocation;

	// 마지막 목격 위치 유효 여부
	UPROPERTY(BlueprintReadOnly, Category = "AI|Target")
	bool bHasLastKnownLocation = false;

	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnUnPossess() override;

	// AI Perception이 Pawn의 눈 위치/회전을 사용하도록 설정
	virtual FVector GetFocalPointOnActor(const AActor* Actor) const override;

	// 타겟 감지 이벤트
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:
	void SetupPerceptionSystem();
};
