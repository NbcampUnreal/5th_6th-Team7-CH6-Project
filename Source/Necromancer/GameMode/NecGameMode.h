// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NecGameMode.generated.h"

class ANecPlayerController;

/**
 * 
 */
UCLASS()
class NECROMANCER_API ANecGameMode : public AGameMode
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<ANecPlayerController>> PlayerControllers;

public:
	void StartGame();
};
