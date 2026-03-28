#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

UENUM(BlueprintType)
enum class ECharacterStatus : uint8
{
    Alive   UMETA(DisplayName = "Alive"),
    Down    UMETA(DisplayName = "Downed"),
    Death   UMETA(DisplayName = "Dead")
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamageReceivedSignature, float, DamageAmount, FVector, HitLocation, bool, bPoiseBroken);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NECROMANCER_API UStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
    UStatComponent();

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;        

    UFUNCTION()
    void OnRep_Health();

    UFUNCTION()
    void HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

    UFUNCTION(NetMulticast, Unreliable)
    void Multicast_OnDamageReceived(float DamageAmount, FVector HitLocation, bool bPoiseBroken);

public:
    UFUNCTION(BlueprintPure)
    float GetCurrentHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintPure)
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintPure)
    float GetArmor() const { return Armor; }

    UFUNCTION(BlueprintPure)
    ECharacterStatus GetStatus() const { return Status; }

    UFUNCTION(BlueprintPure)
    bool GetIsDead() const { return Status == ECharacterStatus::Death; }

    UFUNCTION()
    void SetStatus(ECharacterStatus NewStatus) { Status = NewStatus; }

    UFUNCTION(BlueprintCallable)
    void AddMaxHealth(float Amount);

    UFUNCTION(BlueprintCallable)
    void AddArmor(float Amount);

    UFUNCTION(BlueprintCallable)
    void Heal(float HealAmount);

    void SetCurrentHealth(float NewHealth);

    void BindToOwnerPawn(APawn* NewPawn);

    UPROPERTY(BlueprintAssignable)
    FOnHealthChangedSignature OnHealthChanged;

    UPROPERTY(BlueprintAssignable)
    FOnDamageReceivedSignature OnDamageReceived;

    UPROPERTY(BlueprintAssignable)
    FOnDeathSignature OnDeath;

protected:
    UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Health")
    float CurrentHealth;

    UPROPERTY(ReplicatedUsing = OnRep_Health, EditAnywhere, Category = "Health", meta = (ClampMin = "1.0"))
    float MaxHealth = 100.0f;

    UPROPERTY(Replicated, EditAnywhere, Category = "Armor", meta = (ClampMin = "0.0"))
    float Armor = 0.0f;

    UPROPERTY(ReplicatedUsing = OnRep_Status)
    ECharacterStatus Status =ECharacterStatus::Alive;

    UPROPERTY(EditAnywhere, Category = "Poise", meta = (ClampMin = "0.0"))
    float MaxPoiseTest = 100.0f;

    float CurrentPoiseDamage;
public:
    UFUNCTION()
    void OnRep_Status();

private:
    FTimerHandle DeathTimerHandle;
    float GraceTimeForRevive = 10.0f;
    float CurGraceTimeForRevive = -1;
};