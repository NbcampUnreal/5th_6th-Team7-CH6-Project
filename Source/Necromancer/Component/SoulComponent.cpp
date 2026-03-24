// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SoulComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "Engine/Engine.h"

USoulComponent::USoulComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CurrentHPDrain = BaseHPDrain;

    ReserveBatteries.Empty();

    for (int32 i = 0; i <2; ++i)
    {
        FSoulBattery TestBattery;
        TestBattery.MaxCapacity = 100.f;
        TestBattery.CurrentCapacity = 100.f;
        AddReserveBattery(TestBattery);
    }

    FSoulBattery TestBattery;
    TestBattery.MaxCapacity = 100.f;
    TestBattery.CurrentCapacity = 100.f;
    ActiveBattery = TestBattery;
}

void USoulComponent::BeginPlay()
{
    Super::BeginPlay();

    SetComponentTickEnabled(GetOwner() && GetOwner()->HasAuthority());
}

void USoulComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!GetOwner() || !GetOwner()->HasAuthority())
        return;

    HandleDrain(DeltaTime);
}

void USoulComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(USoulComponent, ActiveBattery);
    DOREPLIFETIME(USoulComponent, ReserveBatteries);
    DOREPLIFETIME(USoulComponent, CurrentState);
}

void USoulComponent::HandleDrain(float DeltaTime)
{
    float DrainThisFrame = DrainPerTick * DeltaTime;
    float Used = ActiveBattery.Consume(DrainThisFrame);

    if (ActiveBattery.IsEmpty())
    {
        SwapReserveToActive();
    }

    if (ActiveBattery.IsEmpty() && ReserveBatteries.Num() == 0)
    {
        EnterDepletedState();
        IncreaseHPDrain(DeltaTime);
    }
    else
    {
        if (CurrentState == ESoulState::Depleted)
        {
            CurrentState = ESoulState::Normal;
        }

        CurrentHPDrain = FMath::Max(
            CurrentHPDrain - (DrainAcceleration * DeltaTime),
            BaseHPDrain
        );
    }
    
}

void USoulComponent::SwapReserveToActive()
{
    if (ReserveBatteries.Num() == 0)
        return;

    ActiveBattery = ReserveBatteries.Pop();
}

void USoulComponent::EnterDepletedState()
{
    if (CurrentState == ESoulState::Depleted)
        return;

    CurrentState = ESoulState::Depleted;

    OnSoulDepleted.Broadcast();
}

void USoulComponent::IncreaseHPDrain(float DeltaTime)
{
    if (CurrentState != ESoulState::Depleted)
        return;

    CurrentHPDrain = FMath::Min(
        CurrentHPDrain + (DrainAcceleration * DeltaTime),
        MaxHPDrain
    );

    AActor* OwnerActor = GetOwner();
    if (!OwnerActor)
        return;

    float DamageThisFrame = CurrentHPDrain * DeltaTime;

    UGameplayStatics::ApplyDamage(
        OwnerActor,
        DamageThisFrame,
        nullptr,
        OwnerActor,
        nullptr
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
}

void USoulComponent::TryRevive()
{

    if (CurrentState != ESoulState::Down)
    {
        return;
    }

    if (!GetOwner())
    {
        return;
    }

    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    if (ReserveBatteries.Num() <= 0)
    {
        return;
    }

    ActiveBattery = ReserveBatteries.Pop();
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

const FSoulBattery& USoulComponent::GetActiveBattery() const
{
    return ActiveBattery;
}

int32 USoulComponent::GetReserveBatteryCount() const
{
    return ReserveBatteries.Num();
}

int32 USoulComponent::GetMaxReserveSlots() const
{
    return MaxReserveSlots;
}

void USoulComponent::CopySoulDataFrom(const USoulComponent* Other)
{
    if (!Other) return;
    ReserveBatteries = Other->ReserveBatteries;
}


/* ===== API ===== */
bool USoulComponent::TakeReserveBattery(FSoulBattery& OutBattery)
{
    if (ReserveBatteries.Num() == 0)
    {
        return false;
    }

    OutBattery = ReserveBatteries.Pop();
    return true;
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
        if (CurrentState == ESoulState::Down) {
            TryRevive();
        }
    }
}