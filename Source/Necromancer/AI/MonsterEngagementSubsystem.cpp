// Fill out your copyright notice in the Description page of Project Settings.

#include "MonsterEngagementSubsystem.h"
#include "MonsterStatComponent.h"

bool UMonsterEngagementSubsystem::RequestAttackSlot(AActor* Monster, AActor* Target)
{
	if (!Monster || !Target)
	{
		return false;
	}

	CleanupStaleEntries();
	CleanupExpiredSlots();

	bool bIsRanged = IsRangedMonster(Monster);
	TMap<TWeakObjectPtr<AActor>, TSet<FSlotInfo>>& SlotMap = bIsRanged ? RangedAttackSlots : MeleeAttackSlots;
	int32 MaxAttackers = bIsRanged ? MaxRangedAttackers : MaxMeleeAttackers;

	TSet<FSlotInfo>& Attackers = SlotMap.FindOrAdd(Target);

	// 이미 슬롯을 가지고 있는지 확인
	for (const FSlotInfo& Info : Attackers)
	{
		if (Info.Monster == Monster)
		{
			return true;
		}
	}

	// 슬롯에 여유가 있으면 추가
	if (Attackers.Num() < MaxAttackers)
	{
		float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
		Attackers.Add(FSlotInfo(Monster, CurrentTime));
		return true;
	}

	// 슬롯 꽉 참 → 거리 비교로 교체 시도
	float NewDistance = GetDistanceToTarget(Monster, Target);

	// 가장 먼 슬롯 보유자 찾기
	FSlotInfo* FarthestSlot = nullptr;
	float FarthestDistance = 0.0f;

	for (FSlotInfo& Info : Attackers)
	{
		if (!Info.Monster.IsValid())
		{
			continue;
		}

		float Dist = GetDistanceToTarget(Info.Monster.Get(), Target);
		if (Dist > FarthestDistance)
		{
			FarthestDistance = Dist;
			FarthestSlot = &Info;
		}
	}

	// 새 요청자가 더 가까우면 교체
	if (FarthestSlot && NewDistance < FarthestDistance)
	{
		Attackers.Remove(*FarthestSlot);
		float CurrentTime = GetWorld() ? GetWorld()->GetTimeSeconds() : 0.0f;
		Attackers.Add(FSlotInfo(Monster, CurrentTime));
		return true;
	}

	return false;
}

void UMonsterEngagementSubsystem::ReleaseAttackSlot(AActor* Monster, AActor* Target)
{
	if (!Monster || !Target)
	{
		return;
	}

	bool bIsRanged = IsRangedMonster(Monster);
	TMap<TWeakObjectPtr<AActor>, TSet<FSlotInfo>>& SlotMap = bIsRanged ? RangedAttackSlots : MeleeAttackSlots;

	if (TSet<FSlotInfo>* Attackers = SlotMap.Find(Target))
	{
		// FSlotInfo에서 Monster만 비교하여 제거
		for (auto It = Attackers->CreateIterator(); It; ++It)
		{
			if (It->Monster == Monster)
			{
				It.RemoveCurrent();
				break;
			}
		}
	}
}

bool UMonsterEngagementSubsystem::HasAttackSlot(AActor* Monster, AActor* Target) const
{
	if (!Monster || !Target)
	{
		return false;
	}

	bool bIsRanged = IsRangedMonster(const_cast<AActor*>(Monster));
	const TMap<TWeakObjectPtr<AActor>, TSet<FSlotInfo>>& SlotMap = bIsRanged ? RangedAttackSlots : MeleeAttackSlots;

	if (const TSet<FSlotInfo>* Attackers = SlotMap.Find(Target))
	{
		for (const FSlotInfo& Info : *Attackers)
		{
			if (Info.Monster == Monster)
			{
				return true;
			}
		}
	}

	return false;
}

int32 UMonsterEngagementSubsystem::GetActiveAttackerCount(AActor* Target) const
{
	if (!Target)
	{
		return 0;
	}

	int32 Count = 0;

	if (const TSet<FSlotInfo>* MeleeAttackers = MeleeAttackSlots.Find(Target))
	{
		Count += MeleeAttackers->Num();
	}

	if (const TSet<FSlotInfo>* RangedAttackers = RangedAttackSlots.Find(Target))
	{
		Count += RangedAttackers->Num();
	}

	return Count;
}

void UMonsterEngagementSubsystem::ReleaseAllSlotsForMonster(AActor* Monster)
{
	if (!Monster)
	{
		return;
	}

	// Melee 슬롯에서 제거
	for (auto& Pair : MeleeAttackSlots)
	{
		for (auto It = Pair.Value.CreateIterator(); It; ++It)
		{
			if (It->Monster == Monster)
			{
				It.RemoveCurrent();
			}
		}
	}

	// Ranged 슬롯에서 제거
	for (auto& Pair : RangedAttackSlots)
	{
		for (auto It = Pair.Value.CreateIterator(); It; ++It)
		{
			if (It->Monster == Monster)
			{
				It.RemoveCurrent();
			}
		}
	}
}

void UMonsterEngagementSubsystem::CleanupStaleEntries()
{
	// Melee 슬롯 정리 (유효하지 않은 액터 제거)
	for (auto It = MeleeAttackSlots.CreateIterator(); It; ++It)
	{
		if (!It->Key.IsValid())
		{
			It.RemoveCurrent();
			continue;
		}

		for (auto SlotIt = It->Value.CreateIterator(); SlotIt; ++SlotIt)
		{
			if (!SlotIt->Monster.IsValid())
			{
				SlotIt.RemoveCurrent();
			}
		}
	}

	// Ranged 슬롯 정리
	for (auto It = RangedAttackSlots.CreateIterator(); It; ++It)
	{
		if (!It->Key.IsValid())
		{
			It.RemoveCurrent();
			continue;
		}

		for (auto SlotIt = It->Value.CreateIterator(); SlotIt; ++SlotIt)
		{
			if (!SlotIt->Monster.IsValid())
			{
				SlotIt.RemoveCurrent();
			}
		}
	}
}

void UMonsterEngagementSubsystem::CleanupExpiredSlots()
{
	if (!GetWorld())
	{
		return;
	}

	float CurrentTime = GetWorld()->GetTimeSeconds();

	// Melee 슬롯 만료 체크
	for (auto& Pair : MeleeAttackSlots)
	{
		for (auto It = Pair.Value.CreateIterator(); It; ++It)
		{
			if (CurrentTime - It->AcquireTime > SlotHoldDuration)
			{
				It.RemoveCurrent();
			}
		}
	}

	// Ranged 슬롯 만료 체크
	for (auto& Pair : RangedAttackSlots)
	{
		for (auto It = Pair.Value.CreateIterator(); It; ++It)
		{
			if (CurrentTime - It->AcquireTime > SlotHoldDuration)
			{
				It.RemoveCurrent();
			}
		}
	}
}

bool UMonsterEngagementSubsystem::IsRangedMonster(AActor* Monster) const
{
	if (!Monster)
	{
		return false;
	}

	UMonsterStatComponent* StatComp = Monster->FindComponentByClass<UMonsterStatComponent>();
	return StatComp && StatComp->GetIsRanged();
}

float UMonsterEngagementSubsystem::GetDistanceToTarget(AActor* Monster, AActor* Target) const
{
	if (!Monster || !Target)
	{
		return FLT_MAX;
	}

	return FVector::Dist(Monster->GetActorLocation(), Target->GetActorLocation());
}
