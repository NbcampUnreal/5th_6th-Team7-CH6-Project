// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "NecWaitingGameMode.generated.h"

/**
 * 
 */
UCLASS()
class NECROMANCER_API ANecWaitingGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	ANecWaitingGameMode();
	
protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<APlayerController>> PlayerControllers;

public:
	void StartGame();
};
