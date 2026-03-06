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

};
