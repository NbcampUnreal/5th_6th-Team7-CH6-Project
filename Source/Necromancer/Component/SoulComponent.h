// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulComponent.generated.h"

UENUM(BlueprintType)
enum class ESoulState : uint8
{
    Normal     UMETA(DisplayName = "Normal"),
    LowPower   UMETA(DisplayName = "LowPower"),
    Depleted   UMETA(DisplayName = "Depleted"),
    Down       UMETA(DisplayName = "Down")
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

// 클라이언트 UI용 델리게이트 (OnRep에서 발동)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnBatteryCountChanged, int32, NewCount);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulCapacityChanged, float, NewPercent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSoulStateChanged, ESoulState, NewState);

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
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:

    /* ===== Batteries ===== */
    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Battery)
    int32 Stack_Battery;
    UPROPERTY(VisibleAnywhere)
    float MaxCapacity = 100.f;
    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Capacity)
    float CurrentCapacity = 100.f;
    /* ===== Drain ===== */

    UPROPERTY(EditAnywhere)
    float DrainPerTick = 1.f;

    UPROPERTY(EditAnywhere)
    float DrainInterval = 1.0f;

    UPROPERTY(EditAnywhere)
    float BaseHPDrain = 2.f;

    UPROPERTY(EditAnywhere)
    float DrainAcceleration = 2.f;

    UPROPERTY(EditAnywhere)
    float MaxHPDrain = 10.f;

    float CurrentHPDrain;

    FTimerHandle DrainTimer;

    /* ===== Down & Revive ===== */

    UPROPERTY(EditAnywhere)
    float ReviveWindow = 10.f;

    UPROPERTY(EditAnywhere)
    float InvincibleDuration = 2.f;

    FTimerHandle DownTimer;
    FTimerHandle InvincibleTimer;

    UPROPERTY(ReplicatedUsing = OnRep_State)
    ESoulState CurrentState = ESoulState::Normal;

    bool bIsInvincible = false;

    // OnRep 콜백
    UFUNCTION()
    void OnRep_Battery();
    UFUNCTION()
    void OnRep_Capacity();
    UFUNCTION()
    void OnRep_State();

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

    // 클라이언트 UI용 (OnRep에서 발동)
    UPROPERTY(BlueprintAssignable)
    FOnBatteryCountChanged OnBatteryCountChanged;

    UPROPERTY(BlueprintAssignable)
    FOnSoulCapacityChanged OnSoulCapacityChanged;

    UPROPERTY(BlueprintAssignable)
    FOnSoulStateChanged OnSoulStateChanged;

    /* ===== API ===== */
    bool TakeReserveBattery();
    void AddReserveBattery();
    void EnterDownState();
    void TryRevive();
    bool IsInvincible() const { return bIsInvincible; }

    UFUNCTION(BlueprintCallable)
    int32 GetReserveBatteryCount() const;

    UFUNCTION(BlueprintCallable)
    float GetCaplcityPercent() const;

    void CopySoulDataFrom(const USoulComponent* Other);
};
