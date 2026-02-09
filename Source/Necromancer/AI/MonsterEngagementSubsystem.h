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
	
	UFUNCTION(BlueprintCallable, Category = "Engagement")
	bool RequestAttackSlot(AActor* Monster, AActor* Target);

	
	UFUNCTION(BlueprintCallable, Category = "Engagement")
	void ReleaseAttackSlot(AActor* Monster, AActor* Target);

	
	UFUNCTION(BlueprintPure, Category = "Engagement")
	bool HasAttackSlot(AActor* Monster, AActor* Target) const;

	
	UFUNCTION(BlueprintPure, Category = "Engagement")
	int32 GetActiveAttackerCount(AActor* Target) const;

	
	UFUNCTION(BlueprintCallable, Category = "Engagement")
	void ReleaseAllSlotsForMonster(AActor* Monster);

	
	UPROPERTY(EditAnywhere, Category = "Engagement")
	int32 MaxMeleeAttackers = 2;

	
	UPROPERTY(EditAnywhere, Category = "Engagement")
	int32 MaxRangedAttackers = 2;

	// 토큰 최대 유지 시간 (초) - 이 시간이 지나면 자동 반환
	UPROPERTY(EditAnywhere, Category = "Engagement")
	float SlotHoldDuration = 5.0f;

private:

	// 슬롯 정보 (획득 시간 포함)
	struct FSlotInfo
	{
		TWeakObjectPtr<AActor> Monster;
		float AcquireTime;

		FSlotInfo()
		{
			AcquireTime = 0.0f;
		}
		FSlotInfo(AActor* InMonster, float InTime)
		{
			Monster = InMonster;
			AcquireTime = InTime;
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

	void CleanupStaleEntries();
	void CleanupExpiredSlots();

	bool IsRangedMonster(AActor* Monster) const;

	// 거리 기반 슬롯 우선순위를 위한 헬퍼
	float GetDistanceToTarget(AActor* Monster, AActor* Target) const;
};
