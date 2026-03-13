// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulComponent.generated.h"


enum class ESoulState
{
    Normal,
    LowPower,
    Depleted,
    Down
};

USTRUCT(BlueprintType)
struct FSoulBattery
{
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    float MaxCapacity = 100.f;
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    float CurrentCapacity = 100.f;

    bool IsEmpty() const
    {
        return CurrentCapacity <= KINDA_SMALL_NUMBER;
    }

    float Consume(float Amount)
    {
        float Used = FMath::Min(CurrentCapacity, Amount);
        CurrentCapacity -= Used;
        return Used;
    }

    void Recharge(float Amount)
    {
        CurrentCapacity = FMath::Clamp(CurrentCapacity + Amount, 0.f, MaxCapacity);
    }

    void SetHalf()
    {
        CurrentCapacity = MaxCapacity * 0.5f;
    }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHPDrainRequested, float, DamageAmount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSoulDepleted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEnterDownState);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReviveRequested);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInvincibleStart);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInvincibleEnd);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NECROMANCER_API USoulComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USoulComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:

    /* ===== Batteries ===== */

    UPROPERTY(VisibleAnywhere)
    FSoulBattery ActiveBattery;

    UPROPERTY(VisibleAnywhere)
    TArray<FSoulBattery> ReserveBatteries;

    UPROPERTY(EditAnywhere)
    int32 MaxReserveSlots = 3;

    /* ===== Drain ===== */

    UPROPERTY(EditAnywhere)
    float DrainPerTick = 0.f;

    UPROPERTY(EditAnywhere)
    float DrainInterval = 1.0f;

    UPROPERTY(EditAnywhere)
    float BaseHPDrain = 5.f;

    UPROPERTY(EditAnywhere)
    float DrainAcceleration = 2.f;

    UPROPERTY(EditAnywhere)
    float MaxHPDrain = 30.f;

    float CurrentHPDrain;

    FTimerHandle DrainTimer;

    /* ===== Down & Revive ===== */

    UPROPERTY(EditAnywhere)
    float ReviveWindow = 10.f;

    UPROPERTY(EditAnywhere)
    float InvincibleDuration = 2.f;

    FTimerHandle DownTimer;
    FTimerHandle InvincibleTimer;

    ESoulState CurrentState = ESoulState::Normal;

    bool bIsInvincible = false;

private:
    void HandleDrain(float DeltaTime);
    void IncreaseHPDrain(float DeltaTime);

    void SwapReserveToActive();
    void EnterDepletedState();
    void RecoverHPDrain(float DeltaTime);

public:

    /* ===== Delegates ===== */

    UPROPERTY(BlueprintAssignable)
    FOnHPDrainRequested OnHPDrainRequested;

    UPROPERTY(BlueprintAssignable)
    FOnSoulDepleted OnSoulDepleted;

    UPROPERTY(BlueprintAssignable)
    FOnEnterDownState OnEnterDownState;

    UPROPERTY(BlueprintAssignable)
    FOnReviveRequested OnReviveRequested;

    UPROPERTY(BlueprintAssignable)
    FOnInvincibleStart OnInvincibleStart;

    UPROPERTY(BlueprintAssignable)
    FOnInvincibleEnd OnInvincibleEnd;

    /* ===== API ===== */
    bool TakeReserveBattery(FSoulBattery& OutBattery);
    void AddReserveBattery(const FSoulBattery& NewBattery);
    void EnterDownState();
    void TryRevive();
    bool IsInvincible() const { return bIsInvincible; }

    UFUNCTION(BlueprintCallable)
    const FSoulBattery& GetActiveBattery() const;
    UFUNCTION(BlueprintCallable)
    int32 GetReserveBatteryCount() const;
    UFUNCTION(BlueprintCallable)
    int32 GetMaxReserveSlots() const;

    void CopySoulDataFrom(const USoulComponent* Other);
};
