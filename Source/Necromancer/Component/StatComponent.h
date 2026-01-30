#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, CurrentHealth, float, MaxHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnDamageReceivedSignature, float, DamageAmount, FVector, HitLocation);

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

public:
    UFUNCTION(BlueprintPure)
    float GetCurrentHealth() const { return CurrentHealth; }

    UFUNCTION(BlueprintPure)
    float GetMaxHealth() const { return MaxHealth; }

    UFUNCTION(BlueprintPure)
    float GetArmor() const { return Armor; }

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
};