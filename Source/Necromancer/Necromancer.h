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

// 전투 역할 (BT 분기용)
UENUM(BlueprintType)
enum class ECombatRole : uint8
{
	Attacker,
	Flanker,
	Waiter
};