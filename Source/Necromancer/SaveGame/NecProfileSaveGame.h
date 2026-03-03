// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "NecProfileSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class NECROMANCER_API UNecProfileSaveGame : public USaveGame
{
	GENERATED_BODY()

public:

    /// <summary>
    /// ÃÑ Àû Ã³Ä¡ È½¼ö
    /// </summary>
    UPROPERTY()
    int32 TotalKillCount;      

	
};
