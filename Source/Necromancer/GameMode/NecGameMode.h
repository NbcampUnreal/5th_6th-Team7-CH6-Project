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
	virtual void Logout(AController* Exiting) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<TObjectPtr<ANecPlayerController>> PlayerControllers;

public:
	void StartGame();
	void InitGameState();

	void EndGame();

	void OnPlayerDeath(ANecPlayerController* DeadPlayerController);
	void OnPlayerRevive(ANecPlayerController* RevivedPlayerController);

	/// <summary>
	/// 관전 대상 반환
	/// </summary>
	/// <param name="RequestPC">사망하여 관전을 요청하는 컨트롤러</param>
	/// <param name="CurSpectatingTarget">
	/// if NULL then 사망해서 자동으로 호출됨(0번째 플레이어 반환),
	/// else then  
	/// </param>
	/// <param name="isPositiveDirection">다음 플레이어를 반환할지, CurSpectatingTarget이 nullptr이면 해당 값은 의미가 없음</param>
	UFUNCTION(Server, Unreliable)
	void Server_ReqeustSpectatingTarget(ANecPlayerController* RequestPC, AActor* CurSpectatingTarget, bool isPositiveDirection);

public:

	/// <summary>
	/// 레벨간 이동시, 데이터 저장을 위해 해당 함수를 호출
	/// 다음 레벨도 이동시 게임모드가 자동으로 저장데이터를 가져와서 셋팅함
	/// </summary>
	UFUNCTION(BlueprintCallable)
	void SaveGameStateToSaveGame();

	UFUNCTION(BlueprintCallable)
	void IncreaseLvDepth();
	int32 GetLvDepth() const;

	void IncreaseKillCount();
	int32 GetKillCount() const;
};
