// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedFriendsGameInstance.h"
#include "NecAFGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class NECROMANCER_API UNecAFGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()



public:
	UFUNCTION(BlueprintImplementableEvent)
	void CreateSession();
	
	UFUNCTION(BlueprintImplementableEvent)
	void InviteFriend();
};
