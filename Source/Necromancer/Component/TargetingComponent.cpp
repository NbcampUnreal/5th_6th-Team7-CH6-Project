#include "Component/TargetingComponent.h"
#include "Character/NecPlayerCharacter.h"
#include "AI/MonsterBase.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blueprint/UserWidget.h"
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

    if (!LockOnWidgetInstance)
    {
        InitLockOnWidget();
    }

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
        AMonsterBase* MonsterTarget = Cast<AMonsterBase>(CurrentTarget);
        if (MonsterTarget && MonsterTarget->GetIsDead())
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

        FVector TargetLocForYaw = TargetLocation;
        TargetLocForYaw.Z = OwnerLocation.Z;        

        FRotator LookAtRot = UKismetMathLibrary::FindLookAtRotation(OwnerLocation, TargetLocForYaw);
        LookAtRot.Pitch = LockOnPitch;

        if (OwnerCharacter && OwnerCharacter->IsLocallyControlled())
        {
            if (AController* PC = OwnerCharacter->GetController())
            {
                FRotator CurrentRot = OwnerCharacter->GetControlRotation();
                FRotator TargetRot = FMath::RInterpTo(CurrentRot, LookAtRot, DeltaTime, 10.0f);

                PC->SetControlRotation(TargetRot);
            }
        }
    }

    UpdateLockOnUI();
}

void UTargetingComponent::ToggleLockOn()
{
    Server_ToggleLockOn();
}

void UTargetingComponent::HandleLockOnInput(FVector2D LookInput)
{
    float CurrentTime = GetWorld()->GetTimeSeconds();
    if (CurrentTime - LastSwitchTime < SwitchCooldown)
    {
        return;
    }

    if (FMath::Abs(LookInput.X) > SwitchThreshold)
    {
        bool bIsRight = LookInput.X > 0;

        Server_SwitchTarget(bIsRight);

        LastSwitchTime = CurrentTime;
    }
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

    if (OwnerCharacter && OwnerCharacter->IsLocallyControlled())
    {
        OnRep_CurrentTarget();
    }
}

void UTargetingComponent::Server_SwitchTarget_Implementation(bool bIsRight)
{
    if (!OwnerCharacter || !CurrentTarget)
    {
        return;
    }

    TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
    ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_Pawn));

    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(OwnerCharacter);
    ActorsToIgnore.Add(CurrentTarget);

    TArray<AActor*> OutActors;

    UKismetSystemLibrary::SphereOverlapActors(
        GetWorld(),
        OwnerCharacter->GetActorLocation(),
        SearchRadius,
        ObjectTypes,
        nullptr,
        ActorsToIgnore,
        OutActors
    );

    AActor* BestNewTarget = nullptr;
    float ClosestAngle = 180.0f;

    FVector CamLoc;
    FRotator CamRot;

    if (OwnerCharacter->GetController())
    {
        OwnerCharacter->GetController()->GetPlayerViewPoint(CamLoc, CamRot);
    }
    else
    {
        CamLoc = OwnerCharacter->GetActorLocation();
        CamRot = OwnerCharacter->GetActorRotation();
    }

    FVector CamForward = CamRot.Vector();
    FVector CamRight = CamRot.RotateVector(FVector::RightVector);

    for (AActor* Candidate : OutActors)
    {
        if (!Candidate)
        {
            continue;
        }

        IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Candidate);
        if (TeamAgent)
        {
            if (TeamAgent->GetGenericTeamId() == OwnerCharacter->GetGenericTeamId())
            {
                continue;
            }
        }

        FVector DirToCandidate = (Candidate->GetActorLocation() - CamLoc).GetSafeNormal();
        FVector DirToCurrent = (CurrentTarget->GetActorLocation() - CamLoc).GetSafeNormal();

        FVector Cross = FVector::CrossProduct(DirToCurrent, DirToCandidate);

        bool bIsCandidateOnRight = Cross.Z > 0;
        if (bIsRight == bIsCandidateOnRight)
        {
            float Dot = FVector::DotProduct(CamForward, DirToCandidate);
            float Angle = FMath::Acos(Dot) * (180.0f / PI);

            if (Angle < ClosestAngle)
            {
                ClosestAngle = Angle;
                BestNewTarget = Candidate;
            }
        }
    }

    if (BestNewTarget)
    {
        CurrentTarget = BestNewTarget;
        
        if (OwnerCharacter)
        {
            OwnerCharacter->SetLockOn(true);
        }

        if (OwnerCharacter && OwnerCharacter->IsLocallyControlled())
        {
            OnRep_CurrentTarget();
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

        if (OwnerCharacter->IsLocallyControlled() && LockOnWidgetInstance)
        {
            if (CurrentTarget)
            {
                LockOnWidgetInstance->SetVisibility(ESlateVisibility::HitTestInvisible);
            }
            else
            {
                LockOnWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
            }
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

            IGenericTeamAgentInterface* TeamAgent = Cast<IGenericTeamAgentInterface>(Actor);
            if (TeamAgent)
            {
                if (TeamAgent->GetGenericTeamId() == OwnerCharacter->GetGenericTeamId())
                {
                    continue;
                }
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

        if (OwnerCharacter->IsLocallyControlled())
        {
            OnRep_CurrentTarget();
        }
    }
}

void UTargetingComponent::InitLockOnWidget()
{
    if (LockOnWidgetInstance || !OwnerCharacter || !LockOnWidgetClass)
    {
        return;
    }

    if (OwnerCharacter->IsLocallyControlled())
    {
        LockOnWidgetInstance = CreateWidget<UUserWidget>(GetWorld(), LockOnWidgetClass);
        if (LockOnWidgetInstance)
        {
            LockOnWidgetInstance->AddToViewport();
            LockOnWidgetInstance->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
            LockOnWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}

void UTargetingComponent::UpdateLockOnUI()
{
    if (!LockOnWidgetInstance)
    {
        return;
    }

    if (!CurrentTarget)
    {
        if (LockOnWidgetInstance->GetVisibility() != ESlateVisibility::Hidden)
        {
            LockOnWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }

        return;
    }

    APlayerController* PC = Cast<APlayerController>(OwnerCharacter->GetController());
    if (!PC)
    {
        return;
    }

    FVector TargetLoc = CurrentTarget->GetActorLocation();

    if (ACharacter* TargetChar = Cast<ACharacter>(CurrentTarget))
    {
        if (TargetChar->GetMesh()->DoesSocketExist(TargetSocketName))
        {
            TargetLoc = TargetChar->GetMesh()->GetSocketLocation(TargetSocketName);
        }
        else
        {
            TargetLoc.Z += 50.0f;
        }
    }

    FVector2D ScreenPos;
    bool bOnScreen = UGameplayStatics::ProjectWorldToScreen(PC, TargetLoc, ScreenPos);

    if (bOnScreen)
    {
        LockOnWidgetInstance->SetPositionInViewport(ScreenPos);

        if (LockOnWidgetInstance->GetVisibility() != ESlateVisibility::HitTestInvisible)
        {
            LockOnWidgetInstance->SetVisibility(ESlateVisibility::HitTestInvisible);
        }
    }
    else
    {
        if (LockOnWidgetInstance->GetVisibility() != ESlateVisibility::Hidden)
        {
            LockOnWidgetInstance->SetVisibility(ESlateVisibility::Hidden);
        }
    }
}