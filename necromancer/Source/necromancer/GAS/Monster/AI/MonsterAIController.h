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
	// Sets default values for this actor's properties
	AMonsterAIController();
	
	UFUNCTION(BlueprintPure,Category="AI|Target")
	AActor* GetCurrentTarget() const {return  CurrentTarget; }
	
	UFUNCTION(BlueprintCallable,Category="AI|Target")
	void SetCurrentTarget(AActor* NewTarget);
	
	UFUNCTION(BlueprintCallable,Category="AI|Target")
	void ClearCurrentTarget();

protected:
	//스테이트 트리 에셋 실행
	UPROPERTY(visibleAnywhere, BlueprintReadOnly, Category = "AI|Components")
	TObjectPtr<UStateTreeAIComponent> StateTreeAIComponent;
	//플레이어 감지
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Components")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComponent;
	//시야 감지 설정
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI|Components")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
	//현재 추적중인 타겟
	UPROPERTY(BlueprintReadOnly,Category="AI|Target")
	TObjectPtr<AActor> CurrentTarget;
	
	virtual void BeginPlay() override;
	//빙의 되었을때
	virtual void OnPossess(APawn* InPawn) override;
	//빙의 풀렸을때
	virtual void OnUnPossess() override;
	
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor,FAIStimulus Stimulus);
	
private:
	
	//perception 컴포 초기화
	void SetupPerceptionSystem();
	
	
	

};
