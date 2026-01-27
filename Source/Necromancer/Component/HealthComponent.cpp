#include "Component/HealthComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"

UHealthComponent::UHealthComponent()
	: CurrentHealth(0.0f)
{
	SetIsReplicatedByDefault(true);
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeDamage);

        CurrentHealth = MaxHealth;

        OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
    }
}

void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UHealthComponent, CurrentHealth);
    DOREPLIFETIME(UHealthComponent, MaxHealth);
    DOREPLIFETIME(UHealthComponent, Armor);
}

void UHealthComponent::OnRep_Health()
{
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || CurrentHealth <= 0.0f)
    {
        return;
    }

    float ActualDamage = FMath::Max(Damage - Armor, 0.0f);
    float NewHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

    SetCurrentHealth(NewHealth);
    OnDamageReceived.Broadcast(ActualDamage, GetOwner()->GetActorLocation());

    if (NewHealth <= 0.0f)
    {
        OnDeath.Broadcast();
    }
}

void UHealthComponent::SetCurrentHealth(float NewHealth)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    CurrentHealth = NewHealth;

    OnRep_Health();
}

void UHealthComponent::AddMaxHealth(float Amount)
{
    if (!GetOwner()->HasAuthority() || Amount == 0.0f)
    {
        return;
    }

    MaxHealth += Amount;
    CurrentHealth += Amount;

    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UHealthComponent::AddArmor(float Amount)
{
    if (!GetOwner()->HasAuthority() || Amount == 0.0f)
    {
        return;
    }

    Armor += Amount;
}

void UHealthComponent::Heal(float HealAmount)
{
    if (!GetOwner()->HasAuthority() || HealAmount <= 0.0f || CurrentHealth <= 0.0f)
    {
        return;
    }

    float NewHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);

    SetCurrentHealth(NewHealth);
}