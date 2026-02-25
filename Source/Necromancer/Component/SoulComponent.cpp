// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SoulComponent.h"
#include "TimerManager.h"

USoulComponent::USoulComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USoulComponent::BeginPlay()
{
    Super::BeginPlay();

    if (!GetOwner()->HasAuthority())
        return;

    CurrentHPDrain = BaseHPDrain;

    // 초기 Active 배터리
    ActiveBattery = FSoulBattery();

    // Reserve 3개 초기 생성
    for (int32 i = 0; i < MaxReserveSlots; i++)
    {
        ReserveBatteries.Add(FSoulBattery());
    }

    GetWorld()->GetTimerManager().SetTimer(
        DrainTimer,
        this,
        &USoulComponent::HandleDrain,
        DrainInterval,
        true
    );
}

void USoulComponent::HandleDrain()
{
    if (!GetOwner()->HasAuthority())
        return;

    float Remaining = DrainPerTick;

    Remaining -= ActiveBattery.Consume(Remaining);

    if (ActiveBattery.IsEmpty())
    {
        SwapReserveToActive();
    }

    if (ActiveBattery.IsEmpty() && ReserveBatteries.Num() == 0)
    {
        EnterDepletedState();

        OnHPDrainRequested.Broadcast(CurrentHPDrain);
        IncreaseHPDrain();
    }
}

void USoulComponent::SwapReserveToActive()
{
    if (ReserveBatteries.Num() == 0)
        return;

    ActiveBattery = ReserveBatteries[0];
    ReserveBatteries.RemoveAt(0);

    CurrentState = ESoulState::LowPower;
}

void USoulComponent::EnterDepletedState()
{
    if (CurrentState == ESoulState::Depleted)
        return;

    CurrentState = ESoulState::Depleted;
    OnSoulDepleted.Broadcast();
}

void USoulComponent::IncreaseHPDrain()
{
    CurrentHPDrain += DrainAcceleration;
    CurrentHPDrain = FMath::Min(CurrentHPDrain, MaxHPDrain);
}

void USoulComponent::EnterDownState()
{
    if (!GetOwner()->HasAuthority())
        return;

    CurrentState = ESoulState::Down;
    OnEnterDownState.Broadcast();

    GetWorld()->GetTimerManager().SetTimer(
        DownTimer,
        this,
        &USoulComponent::TryRevive,
        ReviveWindow,
        false
    );
}

void USoulComponent::TryRevive()
{
    if (!GetOwner()->HasAuthority())
        return;

    if (ReserveBatteries.Num() <= 0)
        return;

    ActiveBattery = ReserveBatteries[0];
    ReserveBatteries.RemoveAt(0);

    ActiveBattery.SetHalf();

    CurrentState = ESoulState::Normal;
    CurrentHPDrain = BaseHPDrain;

    OnReviveRequested.Broadcast();

    bIsInvincible = true;
    OnInvincibleStart.Broadcast();

    GetWorld()->GetTimerManager().SetTimer(
        InvincibleTimer,
        [this]()
        {
            bIsInvincible = false;
            OnInvincibleEnd.Broadcast();
        },
        InvincibleDuration,
        false
    );
}

void USoulComponent::AddReserveBattery(const FSoulBattery& NewBattery)
{
    if (!GetOwner()->HasAuthority())
        return;

    if (ReserveBatteries.Num() >= MaxReserveSlots)
        return;

    ReserveBatteries.Add(NewBattery);

    if (CurrentState == ESoulState::Depleted)
    {
        CurrentHPDrain = BaseHPDrain;
        CurrentState = ESoulState::LowPower;
    }
}