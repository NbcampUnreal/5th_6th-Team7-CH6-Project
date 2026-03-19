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
};
