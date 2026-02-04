#include "Component/TargetingComponent.h"
#include "Character/NecPlayerCharacter.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

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

	if (CurrentTarget && OwnerCharacter)
	{
		float Dist = OwnerCharacter->GetDistanceTo(CurrentTarget);
		if (Dist > SearchRadius * 1.5f)
		{
			ClearLockOn();
		}
	}
}

void UTargetingComponent::ToggleLockOn()
{
	if (CurrentTarget)
	{
		ClearLockOn();
	}
	else
	{
		FindTarget();
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