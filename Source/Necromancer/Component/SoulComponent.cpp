// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/SoulComponent.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Game/NecPlayerState.h"
#include "TimerManager.h"
#include "Engine/Engine.h"

USoulComponent::USoulComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    CurrentHPDrain = BaseHPDrain;

    Stack_Battery = 2;
    CurrentCapacity = MaxCapacity;
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

    DOREPLIFETIME(USoulComponent, CurrentState);

    DOREPLIFETIME(USoulComponent, Stack_Battery);
    //DOREPLIFETIME(USoulComponent, MaxCapacity);
    DOREPLIFETIME(USoulComponent, CurrentCapacity);
}

void USoulComponent::OnRep_Battery()
{
    OnBatteryCountChanged.Broadcast(Stack_Battery);
}

void USoulComponent::OnRep_Capacity()
{
    OnSoulCapacityChanged.Broadcast(CurrentCapacity / MaxCapacity);
}

void USoulComponent::OnRep_State()
{
    OnSoulStateChanged.Broadcast(CurrentState);
}

void USoulComponent::HandleDrain(float DeltaTime)
{
    float DrainThisFrame = DrainPerTick * DeltaTime;
    float Used = FMath::Min(CurrentCapacity, DrainThisFrame);
    CurrentCapacity -= Used;

    if (CurrentCapacity <= KINDA_SMALL_NUMBER)
    {
        SwapReserveToActive();
    }

    if (CurrentCapacity <= KINDA_SMALL_NUMBER && Stack_Battery <= 0)
    {
        EnterDepletedState();
    }
    else
    {
        if (CurrentState == ESoulState::Depleted)
        {
                GetWorld()->GetTimerManager().ClearTimer(HPDrainTimer);
            CurrentState = ESoulState::Normal;
            OnSoulStateChanged.Broadcast(CurrentState);
        }

        CurrentHPDrain = FMath::Max(
            CurrentHPDrain - (DrainAcceleration * DeltaTime),
            BaseHPDrain
        );
    }
    
}

void USoulComponent::SwapReserveToActive()
{
    if (Stack_Battery <= 0)
        return;
    Stack_Battery--;
    CurrentCapacity = MaxCapacity;

    OnBatteryCountChanged.Broadcast(Stack_Battery);
}

void USoulComponent::EnterDepletedState()
{
    if (CurrentState == ESoulState::Depleted)
        return;

    CurrentState = ESoulState::Depleted;
    GetWorld()->GetTimerManager().SetTimer(
        HPDrainTimer,
        this,
        &USoulComponent::IncreaseHPDrain,
        DrainInterval,   // 실행 간격
        true             // 반복
    );
    OnSoulDepleted.Broadcast();
    OnSoulStateChanged.Broadcast(CurrentState);
}

void USoulComponent::IncreaseHPDrain() // Removed DeltaTime parameter
{

    // Accelerate CurrentHPDrain by a fixed amount per tick, not by DeltaTime
    CurrentHPDrain = FMath::Min(
        CurrentHPDrain + (DrainAcceleration * DrainInterval), // Use DrainInterval as the fixed time step
        MaxHPDrain
    );

    APawn* OwnerActor = Cast<APawn>(Cast<APlayerState>(GetOwner())->GetPawn());

    if (!OwnerActor)
        return;

    // Apply a fixed amount of damage per timer tick
    float DamageThisTick = CurrentHPDrain; // Damage is now CurrentHPDrain per DrainInterval

    UGameplayStatics::ApplyDamage(
        OwnerActor,
        DamageThisTick,
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
    OnSoulStateChanged.Broadcast(CurrentState);
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

    if (Stack_Battery <= 0)
    {
        return;
    }
    Stack_Battery--;
    CurrentCapacity = MaxCapacity / 2;

    CurrentState = ESoulState::Normal;
    CurrentHPDrain = BaseHPDrain;

    OnReviveRequested.Broadcast();
    OnBatteryCountChanged.Broadcast(Stack_Battery);
    OnSoulCapacityChanged.Broadcast(CurrentCapacity / MaxCapacity);
    OnSoulStateChanged.Broadcast(CurrentState);

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

int32 USoulComponent::GetReserveBatteryCount() const
{
    return Stack_Battery;
}

float USoulComponent::GetCaplcityPercent() const
{
    return CurrentCapacity/ MaxCapacity;
}

void USoulComponent::CopySoulDataFrom(const USoulComponent* Other)
{
    if (!Other) return;
    Stack_Battery = Other->GetReserveBatteryCount();

}


/* ===== API ===== */
bool USoulComponent::TakeReserveBattery()
{
    if (Stack_Battery <= 0)
    {
        return false;
    }
    Stack_Battery--;
    OnBatteryCountChanged.Broadcast(Stack_Battery);
    return true;
}

void USoulComponent::AddReserveBattery()
{
    Stack_Battery++;
    OnBatteryCountChanged.Broadcast(Stack_Battery);

    if (CurrentState == ESoulState::Depleted)
    {
        CurrentState = ESoulState::Normal;
        OnSoulStateChanged.Broadcast(CurrentState);
    }
    if (CurrentState == ESoulState::Down)
    {
        TryRevive();
    }
}