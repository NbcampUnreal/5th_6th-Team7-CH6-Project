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

	void OnPlayerDeath(ANecPlayerController* DeadPlayerController);

	/// <summary>
	/// 관전 대상을 넘겨줌
	/// </summary>
	/// <param name="RequestPC">요청컨트롤러 == 사망하여 관전중</param>
	/// <param name="CurSpectatingTarget">
	/// if NULL then 0번째 타겟을 관전하도록 클라이언트 RPC실행,
	/// else then 해당 타겟의 앞(뒤) 타겟을 클라이언트 RPC로 넘겨 
	/// </param>
	/// <param name="isPositiveDirection">순회방향, CurSpectatingTarget이 nullptr인경우 해당 값은 의미없음</param>
	UFUNCTION(Server, Unreliable)
	void Server_ReqeustSpectatingTarget(ANecPlayerController* RequestPC, AActor* CurSpectatingTarget, bool isPositiveDirection);
};
