#include "Component/StatComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"

UStatComponent::UStatComponent()
	: CurrentHealth(0.0f)
{
	SetIsReplicatedByDefault(true);
}

void UStatComponent::BeginPlay()
{
	Super::BeginPlay();

    if (GetOwner()->HasAuthority())
    {
        CurrentHealth = MaxHealth;

        OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
    }
}

void UStatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UStatComponent, CurrentHealth);
    DOREPLIFETIME(UStatComponent, MaxHealth);
    DOREPLIFETIME(UStatComponent, Armor);
}

void UStatComponent::OnRep_Health()
{
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UStatComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || CurrentHealth <= 0.0f)
    {
        return;
    }

    float ActualDamage = FMath::Max(Damage - Armor, 0.0f);
    float NewHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

    SetCurrentHealth(NewHealth);
    OnDamageReceived.Broadcast(ActualDamage, GetOwner()->GetActorLocation());

    // AI Perception에 데미지 이벤트 보고 CSM
    if (DamageCauser)
    {
        UAISense_Damage::ReportDamageEvent(GetWorld(),DamagedActor,DamageCauser,ActualDamage,DamageCauser->GetActorLocation(),DamagedActor->GetActorLocation());
    }

    UE_LOG(LogTemp, Warning, TEXT("CurrentHealth: %f"), NewHealth);

    if (NewHealth <= 0.0f)
    {
        OnDeath.Broadcast();
    }
}

void UStatComponent::SetCurrentHealth(float NewHealth)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    CurrentHealth = NewHealth;

    OnRep_Health();
}

void UStatComponent::BindToOwnerPawn(APawn* NewPawn)
{
    if (!GetOwner()->HasAuthority() || !NewPawn)
    {
        return;
    }

    NewPawn->OnTakeAnyDamage.RemoveDynamic(this, &UStatComponent::HandleTakeDamage);
    NewPawn->OnTakeAnyDamage.AddDynamic(this, &UStatComponent::HandleTakeDamage);
}

void UStatComponent::AddMaxHealth(float Amount)
{
    if (!GetOwner()->HasAuthority() || Amount == 0.0f)
    {
        return;
    }

    MaxHealth += Amount;
    CurrentHealth += Amount;

    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UStatComponent::AddArmor(float Amount)
{
    if (!GetOwner()->HasAuthority() || Amount == 0.0f)
    {
        return;
    }

    Armor += Amount;
}

void UStatComponent::Heal(float HealAmount)
{
    if (!GetOwner()->HasAuthority() || HealAmount <= 0.0f || CurrentHealth <= 0.0f)
    {
        return;
    }

    float NewHealth = FMath::Clamp(CurrentHealth + HealAmount, 0.0f, MaxHealth);

    SetCurrentHealth(NewHealth);
}