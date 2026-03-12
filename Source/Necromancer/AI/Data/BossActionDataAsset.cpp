#include "BossActionDataAsset.h"

const FBossActionEntry* UBossActionDataAsset::SelectAction(int32 CurrentPhase, float DistanceToTarget,
	const TMap<int32, float>& CooldownTimers, float CurrentTime, int32& OutIndex) const
{
	TArray<TPair<int32, float>> Candidates; // <인덱스, 가중치>
	float TotalWeight = 0.0f;

	for (int32 i = 0; i < Actions.Num(); ++i)
	{
		const FBossActionEntry& Entry = Actions[i];

		// 페이즈 필터
		if (CurrentPhase < Entry.MinPhase || CurrentPhase > Entry.MaxPhase)
		{
			continue;
		}

		// Melee 타입: 거리 필터
		if (Entry.ActionType == EBossActionType::Melee)
		{
			if (DistanceToTarget < Entry.MinDistance || DistanceToTarget > Entry.MaxDistance)
			{
				continue;
			}
		}

		// 쿨타임 필터
		if (Entry.Cooldown > 0.0f)
		{
			const float* LastUsedTime = CooldownTimers.Find(i);
			if (LastUsedTime && (CurrentTime - *LastUsedTime) < Entry.Cooldown)
			{
				continue;
			}
		}

		Candidates.Add(TPair<int32, float>(i, Entry.Weight));
		TotalWeight += Entry.Weight;
	}

	if (Candidates.Num() == 0 || TotalWeight <= 0.0f)
	{
		OutIndex = INDEX_NONE;
		return nullptr;
	}

	// 가중치 랜덤 선택
	float RandomValue = FMath::FRandRange(0.0f, TotalWeight);
	float AccumulatedWeight = 0.0f;

	for (const auto& Candidate : Candidates)
	{
		AccumulatedWeight += Candidate.Value;
		if (RandomValue <= AccumulatedWeight)
		{
			OutIndex = Candidate.Key;
			return &Actions[Candidate.Key];
		}
	}

	// 부동소수점 오차 방지: 마지막 후보 반환
	OutIndex = Candidates.Last().Key;
	return &Actions[Candidates.Last().Key];
}
