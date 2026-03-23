// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "NecGameState.generated.h"


UENUM(BlueprintType)
enum class ESessionState : uint8
{
	None,
	Waiting,
	Playing,
	Ending,
	End
};



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerControllerCountChanged, int32, NewCount);
/**
 * 
 */
UCLASS()
class NECROMANCER_API ANecGameState : public AGameState
{
	GENERATED_BODY()
	
public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
public:
	/// <summary>
	/// Deprecated: WaitingLv 로 인해서 더이상 세션 상태를 관리할필요가 없음
	/// </summary>
//	UPROPERTY(ReplicatedUsing = OnRep_SessionState)
//	ESessionState SessionState = ESessionState::Waiting;
//
//public:
//	UFUNCTION()
//	void OnRep_SessionState();

public:
	UPROPERTY(ReplicatedUsing = OnRep_PlayerControllerCount)
	int32 PlayerControllerCount = 0;
	
	FOnPlayerControllerCountChanged OnPlayerControllerCountChanged;
public:
	UFUNCTION()
	void OnRep_PlayerControllerCount();


/// <summary>
/// 호스트의 게임세션 정보를 GameMode 를 이용하여 저장
/// 
/// 해당 프로세스에 변수를 추가하고싶다면
/// 	1. 게임스테이트 변수추가
/// 	2. 게임스테이트 복제변수 추가
/// 	3. 세션세이브게임에 변수추가
/// 	4. UNecSaveGameSubsystem- SaveSessionData()에 변수 추가
/// 	5. ANecGameMode- InitGameState()에 변수추가
/// </summary>
public:
	UPROPERTY(Replicated)
	int32 LvDepth = 0;

	UPROPERTY(Replicated)
	int32 SubmittedItemValue = 0;

	UPROPERTY(Replicated)
	int32 KillCount = 0;
};
