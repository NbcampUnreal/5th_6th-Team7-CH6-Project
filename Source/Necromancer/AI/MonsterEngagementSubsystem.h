// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "MonsterEngagementSubsystem.generated.h"


UCLASS()
class NECROMANCER_API UMonsterEngagementSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// 공격 슬롯 요청 (근접/원거리 자동 판별)
	UFUNCTION(BlueprintCallable, Category = "Engagement")
	bool RequestAttackSlot(AActor* Monster, AActor* Target);

	// 공격 슬롯 반환
	UFUNCTION(BlueprintCallable, Category = "Engagement")
	void ReleaseAttackSlot(AActor* Monster, AActor* Target);

	// 슬롯 보유 여부 확인
	UFUNCTION(BlueprintPure, Category = "Engagement")
	bool HasAttackSlot(AActor* Monster, AActor* Target) const;

	// 타겟에 대한 현재 공격자 수
	UFUNCTION(BlueprintPure, Category = "Engagement")
	int32 GetActiveAttackerCount(AActor* Target) const;

	// 몬스터의 모든 슬롯 일괄 반환 (사망 등)
	UFUNCTION(BlueprintCallable, Category = "Engagement")
	void ReleaseAllSlotsForMonster(AActor* Monster);

	UPROPERTY(EditAnywhere, Category = "Engagement")
	int32 MaxMeleeAttackers = 2;

	UPROPERTY(EditAnywhere, Category = "Engagement")
	int32 MaxRangedAttackers = 2;

	// 토큰 최대 유지 시간 (초) - 이 시간이 지나면 자동 반환
	UPROPERTY(EditAnywhere, Category = "Engagement")
	float SlotHoldDuration = 5.0f;

	// 공격 완료 후 슬롯 유지 시간 (초) - 이 시간 경과 + 만석이면 양보
	UPROPERTY(EditAnywhere, Category = "Engagement")
	float SlotRetentionTime = 30.0f;

	// 공격 완료 후 슬롯 유지 (첫 공격 시 LastAttackTime 기록, AcquireTime 갱신)
	UFUNCTION(BlueprintCallable, Category = "Engagement")
	void RefreshSlotAfterAttack(AActor* Monster, AActor* Target);

	// 양보 판정 (30초 경과 + 만석이면 true)
	UFUNCTION(BlueprintPure, Category = "Engagement")
	bool ShouldYieldSlot(AActor* Monster, AActor* Target) const;

	// 피격 시 임시 슬롯 부여 (MaxAttackers 무시, 이미 보유 중이면 무시)
	UFUNCTION(BlueprintCallable, Category = "Engagement")
	void GrantTemporarySlot(AActor* Monster, AActor* Target);

private:

	// 슬롯 정보 (획득 시간 + 첫 공격 시간 포함)
	struct FSlotInfo
	{
		TWeakObjectPtr<AActor> Monster;
		float AcquireTime;
		float LastAttackTime;

		FSlotInfo()
		{
			AcquireTime = 0.0f;
			LastAttackTime = 0.0f;
		}
		FSlotInfo(AActor* InMonster, float InTime)
		{
			Monster = InMonster;
			AcquireTime = InTime;
			LastAttackTime = 0.0f;
		}
		//해시 비교를 위한 비교 연산자 오버라이딩
		bool operator == (const FSlotInfo& Other) const
		{
			return Monster == Other.Monster;
		}
		//friend는 전역 함수이지만 내부에 접근 가능 
		friend uint32 GetTypeHash(const FSlotInfo& Info)
		{
			return GetTypeHash(Info.Monster);
		}
	};

	TMap<TWeakObjectPtr<AActor>, TSet<FSlotInfo>> MeleeAttackSlots;
	TMap<TWeakObjectPtr<AActor>, TSet<FSlotInfo>> RangedAttackSlots;

	// 유효하지 않은 슬롯 정리
	void CleanupStaleEntries();
	// 만료된 슬롯 자동 반환
	void CleanupExpiredSlots();

	bool IsRangedMonster(AActor* Monster) const;
	float GetDistanceToTarget(AActor* Monster, AActor* Target) const;
};
