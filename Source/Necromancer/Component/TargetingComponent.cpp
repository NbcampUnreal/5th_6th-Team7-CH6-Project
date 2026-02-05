#include "Component/TargetingComponent.h"
#include "Character/NecPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "GameFramework/CharacterMovementComponent.h"

UTargetingComponent::UTargetingComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UTargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	OwnerCharacter = Cast<ANecPlayerCharacter>(GetOwner());
}

void UTargetingComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UTargetingComponent, CurrentTarget);
}

void UTargetingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (!IsValid(OwnerCharacter))
    {
        return;
    }

    bool bHasTarget = IsValid(CurrentTarget);
    bool bIsMovementOriented = OwnerCharacter->GetCharacterMovement()->bOrientRotationToMovement;

    if (bHasTarget && bIsMovementOriented)
    {
        OwnerCharacter->SetLockOn(true);
    }
    else if (!bHasTarget && !bIsMovementOriented)
    {
        OwnerCharacter->SetLockOn(false);
    }

    if (bHasTarget)
    {
        float Dist = OwnerCharacter->GetDistanceTo(CurrentTarget);
        if (Dist > SearchRadius * 1.5f)
        {
            if (OwnerCharacter->HasAuthority())
            {
                ClearLockOn();
            }
            else
            {
                Server_ToggleLockOn();
            }

            return;
        }

        FVector TargetLocation = CurrentTarget->GetActorLocation();
        FVector OwnerLocation = OwnerCharacter->GetActorLocation();

        TargetLocation.Z = OwnerLocation.Z;

        FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, TargetLocation);

        if (AController* PC = OwnerCharacter->GetController())
        {
            FRotator CurrentRot = OwnerCharacter->GetControlRotation();
            FRotator TargetRot = FMath::RInterpTo(CurrentRot, LookAtRot, DeltaTime, 10.0f);

            PC->SetControlRotation(TargetRot);
        }
    }    
}

void UTargetingComponent::ToggleLockOn()
{
    Server_ToggleLockOn();
}

void UTargetingComponent::Server_ToggleLockOn_Implementation()
{
    if (CurrentTarget)
    {
        ClearLockOn();
    }
    else
    {
        FindTarget();

        if (CurrentTarget && OwnerCharacter)
        {
            OwnerCharacter->SetLockOn(true);
        }
    }
}

void UTargetingComponent::OnRep_CurrentTarget()
{
    if (OwnerCharacter)
    {
        if (CurrentTarget)
        {
            OwnerCharacter->SetLockOn(true);
        }
        else
        {
            OwnerCharacter->SetLockOn(false);
        }
    }
}

void UTargetingComponent::FindTarget()
{
    if (!OwnerCharacter || !OwnerCharacter->GetController())
    {
        return;
    }

    FVector Start = OwnerCharacter->GetActorLocation();
    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(OwnerCharacter);

    TArray<AActor*> OutActors;

    bool bResult = UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        Start,
        SearchRadius,
        ObjectTypes,
        nullptr,
        ActorsToIgnore,
        OutActors
    );

    if (bResult)
    {
        AActor* ClosestTarget = nullptr;
        float ClosestDist = FLT_MAX;

        FVector CamLoc;
        FRotator CamRot;
        OwnerCharacter->GetController()->GetPlayerViewPoint(CamLoc, CamRot);
        FVector CamForward = CamRot.Vector();

        for (AActor* Actor : OutActors)
        {
            if (!Actor)
            {
                continue;
            }

            FHitResult HitResult;
            bool bVisible = GetWorld()->LineTraceSingleByChannel(
                HitResult,
                Start,
                Actor->GetActorLocation(),
                ECC_Visibility
            );

            if (!bVisible || HitResult.GetActor() == Actor)
            {
                float Dist = FVector::Dist(Start, Actor->GetActorLocation());

                FVector DirectionToTarget = (Actor->GetActorLocation() - Start).GetSafeNormal();
                float Dot = FVector::DotProduct(CamForward, DirectionToTarget);

                if (Dot > 0.5f && Dist < ClosestDist)
                {
                    ClosestDist = Dist;
                    ClosestTarget = Actor;
                }
            }
        }

        if (ClosestTarget)
        {
            CurrentTarget = ClosestTarget;
            OwnerCharacter->SetLockOn(true);
        }
    }
}

void UTargetingComponent::ClearLockOn()
{
	CurrentTarget = nullptr;

    if (OwnerCharacter)
    {
        OwnerCharacter->SetLockOn(false);
    }
}