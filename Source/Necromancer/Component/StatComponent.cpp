#include "Component/StatComponent.h"
#include "GameFramework/Actor.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Damage.h"
#include "GenericTeamAgentInterface.h"
#include "Character/NecPlayerCharacter.h"
#include "Component/CombatComponent.h"
#include "Component/StaminaComponent.h"
#include "Item/Weapon_Item_Base.h"

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
        if (CurrentHealth <= 0.0f)
        {
            CurrentHealth = MaxHealth;
        }

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

    float ActualDamage = Damage;

    ANecPlayerCharacter* PlayerCharacter = Cast<ANecPlayerCharacter>(DamagedActor);
    if (PlayerCharacter)
    {
        UCombatComponent* CombatComp = PlayerCharacter->FindComponentByClass<UCombatComponent>();
        if (CombatComp && CombatComp->IsGuarding())
        {
            ActualDamage *= CombatComp->GetCurrentWeapon()->GetGuardRate();

            UStaminaComponent* StaminaComp = PlayerCharacter->GetStaminaComponent();
            if (StaminaComp)
            {
                StaminaComp->ConsumeStamina(30.0f);
            }
        }
    }

    ActualDamage = FMath::Max(ActualDamage - Armor, 0.0f);
    float NewHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

    SetCurrentHealth(NewHealth);
    OnDamageReceived.Broadcast(ActualDamage, GetOwner()->GetActorLocation());
    
    if (DamageCauser)
    {
        bool bFriendlyFire = false;
        if (InstigatedBy)
        {
            IGenericTeamAgentInterface* InstigatorTeam = Cast<IGenericTeamAgentInterface>(InstigatedBy);
            IGenericTeamAgentInterface* VictimTeam = Cast<IGenericTeamAgentInterface>(DamagedActor);
            if (InstigatorTeam && VictimTeam &&
                InstigatorTeam->GetGenericTeamId() == VictimTeam->GetGenericTeamId())
            {
                bFriendlyFire = true;
            }
        }

        if (!bFriendlyFire)
        {
            UAISense_Damage::ReportDamageEvent(GetWorld(),DamagedActor,DamageCauser,ActualDamage,DamageCauser->GetActorLocation(),DamagedActor->GetActorLocation());
        }
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