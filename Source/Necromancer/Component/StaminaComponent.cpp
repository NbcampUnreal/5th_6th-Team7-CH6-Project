#include "Component/StaminaComponent.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Character/NecPlayerCharacter.h"
#include "Controller/NecPlayerController.h"
#include "Game/NecPlayerState.h"

UStaminaComponent::UStaminaComponent()
	: CurrentStamina(0.0f)
	, StaminaRecoveryRate(20.0f)
	, CurrentDrainRate(0.0f)
{
	SetIsReplicatedByDefault(true);

	PrimaryComponentTick.bCanEverTick = true;
}

void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner()->HasAuthority())
	{
		CurrentStamina = MaxStamina;

		OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
	}
}

void UStaminaComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UStaminaComponent, CurrentStamina);
	DOREPLIFETIME(UStaminaComponent, bIsExhausted);
}

void UStaminaComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    bool bIsDraining = CurrentDrainRate > 0.0f;
    bool bIsMoving = false;

    if (APlayerState* PS = Cast<APlayerState>(GetOwner()))
    {
        if (APawn* OwningPawn = PS->GetPawn())
        {
            bIsMoving = OwningPawn->GetVelocity().Size() > 5.0f;
        }
    }

    float NewStamina = CurrentStamina;

    if (bIsDraining && bIsMoving)
    {
        NewStamina -= CurrentDrainRate * DeltaTime;
    }
    else
    {
        NewStamina += StaminaRecoveryRate * DeltaTime;
    }

    NewStamina = FMath::Clamp(NewStamina, 0.0f, MaxStamina);

    if (bIsExhausted)
    {
        if (NewStamina >= RecoveryThreshold)
        {
            bIsExhausted = false;
        }
    }
    else
    {
        if (NewStamina <= 0.0f && bIsDraining)
        {
            bIsExhausted = true;
        }
    }

    if (GetOwner()->HasAuthority())
    {
        SetStamina(NewStamina);

        if (bIsExhausted && bIsDraining)
        {
            if (APlayerState* PS = Cast<APlayerState>(GetOwner()))
            {
                if (ANecPlayerCharacter* PlayerCharacter = Cast<ANecPlayerCharacter>(PS->GetPawn()))
                {
                    PlayerCharacter->Server_SetSprint(false);
                }
            }
        }
    }
    else
    {
        CurrentStamina = NewStamina;
        OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
    }
}

void UStaminaComponent::OnRep_CurrentStamina()
{
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

void UStaminaComponent::OnRep_IsExhausted()
{
    if (bIsExhausted)
    {
        StopStaminaDrain(false);
    }
}

bool UStaminaComponent::ConsumeStamina(float Amount)
{
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}

	if (CurrentStamina >= Amount)
	{
		SetStamina(CurrentStamina - Amount);

		return true;
	}

	return false;
}

void UStaminaComponent::StartStaminaDrain(float DrainRate)
{
    CurrentDrainRate = FMath::Max(0.0f, DrainRate);
}

void UStaminaComponent::StopStaminaDrain(bool bResetExhaustion)
{
    CurrentDrainRate = 0.0f;
    
    if (GetOwner()->HasAuthority() && bResetExhaustion)
	{
		bIsExhausted = false;
	}
}

void UStaminaComponent::ConsumeStamina_Predictive(float Amount)
{
    if (GetOwner()->HasAuthority())
    {
        return;
    }

    if (CurrentStamina >= Amount)
    {
        CurrentStamina -= Amount;

        OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
    }
}

void UStaminaComponent::SetStamina(float NewStamina)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}

	CurrentStamina = NewStamina;

	OnRep_CurrentStamina();
}