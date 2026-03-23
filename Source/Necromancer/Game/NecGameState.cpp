// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/NecGameState.h"

#include "Net/UnrealNetwork.h"

void ANecGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(ThisClass, SessionState);
	DOREPLIFETIME(ThisClass, PlayerControllerCount);
	DOREPLIFETIME(ThisClass, LvDepth);
	DOREPLIFETIME(ThisClass, KillCount);
}

//void ANecGameState::OnRep_SessionState()
//{
//
//}

void ANecGameState::OnRep_PlayerControllerCount()
{
	OnPlayerControllerCountChanged.Broadcast(PlayerControllerCount);
}
