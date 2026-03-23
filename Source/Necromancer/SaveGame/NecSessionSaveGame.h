// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "NecSessionSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class NECROMANCER_API UNecSessionSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	int32 LvDepth = 0;

	UPROPERTY()
	int32 SubmittedItemValue = 0;

	/// <summary>
	/// 팀 전체가 사냥한 몹 수
	/// </summary>
	UPROPERTY()
	int32 KillCount = 0;

};
