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
#include "Controller/NecPlayerController.h"
#include "Game/NecPlayerState.h"
#include "DamageType/NecDamageType.h"
#include "AI/MonsterBase.h"

UStatComponent::UStatComponent()
	: CurrentHealth(0.0f)
    , CurrentPoiseDamage(0.0f)
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
    DOREPLIFETIME(UStatComponent, Status);
}

void UStatComponent::OnRep_Health()
{
    OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

void UStatComponent::OnRep_Status()
{
    switch (Status)
    {
    case ECharacterStatus::Alive:
        if (GetOwner()->HasAuthority())
        {
            GetWorld()->GetTimerManager().ClearTimer(DeathTimerHandle);
        }
        break;

    case ECharacterStatus::Down:
        if (GetOwner()->HasAuthority())
        {
            GetWorld()->GetTimerManager().ClearTimer(DeathTimerHandle);

            GetWorld()->GetTimerManager().SetTimer(
                DeathTimerHandle,
                FTimerDelegate::CreateLambda([this]()
                    {
                        if (!IsValid(this))
                        {
                            return;
                        }

                        if (Status != ECharacterStatus::Down)
                        {
                            return;
                        }

                        Status = ECharacterStatus::Death;
                        OnRep_Status();
                    }),
                10.0f,
                false
            );
        }
        break;

    case ECharacterStatus::Death:
    { 
        if (GetOwner()->HasAuthority())
        {
            GetWorld()->GetTimerManager().ClearTimer(DeathTimerHandle);
        }
        APlayerState* PS = Cast<APlayerState>(GetOwner());
        if (PS == nullptr) return;

        ANecPlayerCharacter* MyCharacter = Cast<ANecPlayerCharacter>(PS->GetPawn());
        if (MyCharacter == nullptr) return;

        ANecPlayerController* OwnerController = Cast<ANecPlayerController>(MyCharacter->GetController());
        if (OwnerController == nullptr) return;

        OwnerController->OnPlayerDeath();
        break;
    }

    default:
        // 예외 처리
        break;
    }
}

void UStatComponent::HandleTakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || CurrentHealth <= 0.0f)
    {
        return;
    }

    float ActualDamage = Damage;
    float IncomingPoiseDamage = Damage;

    if (AWeapon_Item_Base* Weapon = Cast<AWeapon_Item_Base>(DamageCauser))
    {
        IncomingPoiseDamage = Weapon->GetPoiseDamage();
    }
    else if (AMonsterBase* Monster = Cast<AMonsterBase>(DamageCauser))
    {
        IncomingPoiseDamage = Monster->GetPoiseDamage();
    }
    else if (const UNecDamageType* NecDamage = Cast<UNecDamageType>(DamageType))
    {
        IncomingPoiseDamage = NecDamage->PoiseDamage;
    }

    if (DamagedActor)
    {
        ANecPlayerCharacter* PlayerCharacter = Cast<ANecPlayerCharacter>(DamagedActor);
        if (PlayerCharacter)
        {
            UCombatComponent* CombatComp = PlayerCharacter->FindComponentByClass<UCombatComponent>();
            if (CombatComp && CombatComp->IsGuarding())
            {
                float GuardRate = CombatComp->GetCurrentWeapon()->GetGuardRate();
                ActualDamage *= GuardRate;
                //IncomingPoiseDamage *= GuardRate;

                UStaminaComponent* StaminaComp = PlayerCharacter->GetStaminaComponent();
                if (StaminaComp)
                {
                    StaminaComp->ConsumeStamina(30.0f);
                }
            }
        }
    }

    // 몬스터 블로킹 시 데미지 + 포이즈 감소
    AMonsterBase* MonsterCharacter = Cast<AMonsterBase>(DamagedActor);
    if (MonsterCharacter && MonsterCharacter->IsBlocking())
    {
        ActualDamage *= (1.0f - MonsterCharacter->ShieldGuardRate);
        IncomingPoiseDamage *= (1.0f - MonsterCharacter->ShieldGuardRate);
    }

    ActualDamage = FMath::Max(ActualDamage - Armor, 0.0f);
    float NewHealth = FMath::Clamp(CurrentHealth - ActualDamage, 0.0f, MaxHealth);

    SetCurrentHealth(NewHealth);

    // Poise Damage
    bool bPoiseBroken = false;
    CurrentPoiseDamage += IncomingPoiseDamage;

    if (CurrentPoiseDamage >= MaxPoiseTest)
    {
        bPoiseBroken = true;
        CurrentPoiseDamage = 0.0f;
    }

    FVector HitLocation = DamagedActor ? DamagedActor->GetActorLocation() : FVector::ZeroVector;
    Multicast_OnDamageReceived(ActualDamage, HitLocation, bPoiseBroken);
    
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
        if (InstigatedBy)
        {
            ANecPlayerController* PC = Cast<ANecPlayerController>(InstigatedBy);
            if (PC)
            {
                PC->Client_NotifyMonsterKill();
            }
        }
        Status = ECharacterStatus::Down;
        OnRep_Status();

        OnDeath.Broadcast();
    }
}

void UStatComponent::Multicast_OnDamageReceived_Implementation(float DamageAmount, FVector HitLocation, bool bPoiseBroken)
{
    OnDamageReceived.Broadcast(DamageAmount, HitLocation, bPoiseBroken);
}

void UStatComponent::SetCurrentHealth(float NewHealth)
{
    if (!GetOwner()->HasAuthority())
    {
        return;
    }

    CurrentHealth = NewHealth;
    if (CurrentHealth > 0) {
        Status = ECharacterStatus::Alive;
    }
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