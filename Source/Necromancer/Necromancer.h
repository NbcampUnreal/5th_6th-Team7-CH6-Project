// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#define TEAM_ID_PLAYER 1
#define TEAM_ID_MONSTER 2

#define NAME_TargetActor "TargetActor"
#define NAME_LastLocation "LastLocation"
#define NAME_IsStaggered "IsStaggered"

#define NAME_MoveToLocation "MoveToLocation"
#define NAME_InAttackRange "IsInAttackRange"
#define NAME_IsRangedMonster "IsRangedMonster"
#define NAME_IsTooClose "IsTooClose"
#define NAME_IsAttacking "IsAttacking"
#define NAME_HasAttackSlot "HasAttackSlot"

#define NAME_SpawnLocation "MonsterSpawnLocation"
#define NAME_ShouldReturnToSpawn "ShouldReturnToSpawn"

#define NAME_PatrolLocation "PatrolLocation"

#define NAME_CanPropagateAggro "CanPropagateAggro"

// 전투 리액션/역할 시스템
#define NAME_PlayerIsAttacking "PlayerIsAttacking"
#define NAME_CombatRole "CombatRole"
#define NAME_BestAction "BestAction"

// 보스 페이즈
#define NAME_BossPhase "BossPhase"

// 전투 역할 (BT 분기용)
UENUM(BlueprintType)
enum class ECombatRole : uint8
{
	Attacker,
	Flanker,
	Waiter
};

// 보스 액션 타입
UENUM(BlueprintType)
enum class EBossActionType : uint8
{
	Melee,		// 근접 공격 (기존 AN_MonsterAttack으로 판정)
	AOE,		// 범위 공격 (AN_BossAOEDamage로 판정)
	Teleport	// 순간이동 (이동 후 옵션 몽타주)
};

// 순간이동 모드
UENUM(BlueprintType)
enum class ETeleportMode : uint8
{
	BehindTarget,	// 타겟 뒤쪽으로 순간이동
	RandomAround	// 타겟 주변 랜덤 위치로 순간이동
};

// 몬스터 AI 로그 카테고리
DECLARE_LOG_CATEGORY_EXTERN(LogMonsterAI, Log, All);