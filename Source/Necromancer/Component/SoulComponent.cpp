// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SoulComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

USoulComponent::USoulComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    CurrentHPDrain = BaseHPDrain;
}

void USoulComponent::BeginPlay()
{
    Super::BeginPlay();


    ReserveBatteries.Empty();

    for (int32 i = 0; i < MaxReserveSlots; ++i)
    {
        FSoulBattery TestBattery;
        TestBattery.MaxCapacity = 100.f;
        TestBattery.CurrentCapacity = 100.f;

        ReserveBatteries.Add(TestBattery);
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
    if (CurrentState == ESoulState::Down)
        return;

    float Used = ActiveBattery.Consume(DrainPerTick);
    if (ActiveBattery.IsEmpty())
    {
        SwapReserveToActive();
    }
    if (ActiveBattery.IsEmpty() && ReserveBatteries.Num() == 0)
    {
        EnterDepletedState();
        OnSoulDepleted.Broadcast();
        IncreaseHPDrain();
    }
    else {
        if (CurrentState == ESoulState::Depleted)
        {
            CurrentState = ESoulState::Normal;
        }

        CurrentHPDrain = FMath::Max(
            CurrentHPDrain - DrainAcceleration,
            BaseHPDrain
        );
    }

    
}

void USoulComponent::SwapReserveToActive()
{
    if (ReserveBatteries.Num() == 0)
        return;

    ActiveBattery = ReserveBatteries[0];
    ReserveBatteries.RemoveAt(0);

    //CurrentState = ESoulState::LowPower;
}

void USoulComponent::EnterDepletedState()
{
    if (CurrentState == ESoulState::Depleted)
        return;

    CurrentState = ESoulState::Depleted;
}

void USoulComponent::IncreaseHPDrain()
{
    if (CurrentState != ESoulState::Depleted)
        return;

    CurrentHPDrain = FMath::Min(
        CurrentHPDrain + DrainAcceleration,
        MaxHPDrain
    );

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
        return;

    UGameplayStatics::ApplyDamage(
        OwnerActor,
        CurrentHPDrain,
        nullptr,
        OwnerActor,
        nullptr // 필요하면 전용 DamageType 지정
    );
}

void USoulComponent::RecoverHPDrain(float DeltaTime)
{
    if (CurrentHPDrain <= BaseHPDrain)
    {
        CurrentHPDrain = BaseHPDrain;
        return;
    }

    CurrentHPDrain -= DrainAcceleration * DeltaTime;

    if (CurrentHPDrain < BaseHPDrain)
    {
        CurrentHPDrain = BaseHPDrain;
    }
}

void USoulComponent::EnterDownState()
{
    if (CurrentState == ESoulState::Down)
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

    //ActiveBattery = ReserveBatteries[0];
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
    if (ReserveBatteries.Num() < MaxReserveSlots)
    {
        ReserveBatteries.Add(NewBattery);

        if (CurrentState == ESoulState::Depleted)
        {
            CurrentState = ESoulState::Normal;

        }
    }
}