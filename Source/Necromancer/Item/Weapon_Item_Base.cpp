//Weapon_Item_Base.cpp

#include "Weapon_Item_Base.h"

#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sword_Item.h"
#include "Kismet/KismetMathLibrary.h"

AWeapon_Item_Base::AWeapon_Item_Base()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = false;

    bReplicates = true;
    SetReplicateMovement(true);

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
    SetRootComponent(WeaponMesh);

    WeaponMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void AWeapon_Item_Base::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsAttacking)
    {
        PerformTrace();
    }
}

void AWeapon_Item_Base::StartAttack()
{
    bIsAttacking = true;
    HitActors.Empty();

    FVector StartPos = WeaponMesh->GetSocketLocation(SocketNameStart);
    FVector EndPos = WeaponMesh->GetSocketLocation(SocketNameEnd);

    LastCenterLocation = (StartPos + EndPos) * 0.5f;

    SetActorTickEnabled(true);

    if (AttackSound)
    {
        UGameplayStatics::PlaySoundAtLocation(this, AttackSound, GetActorLocation());
    }
}

void AWeapon_Item_Base::EndAttack()
{
    bIsAttacking = false;
    SetActorTickEnabled(false);
}

void AWeapon_Item_Base::PerformTrace()
{    
    FVector StartSocket = WeaponMesh->GetSocketLocation(SocketNameStart);
    FVector EndSocket = WeaponMesh->GetSocketLocation(SocketNameEnd);

    FVector CurrentCenterLocation = (StartSocket + EndSocket) * 0.5f;

    FVector Direction = EndSocket - StartSocket;
    FRotator BoxRotation = UKismetMathLibrary::MakeRotFromX(Direction);

    float WeaponLength = Direction.Size();
    FVector BoxHalfSize = FVector(WeaponLength * 0.5f, TraceExtent.Y, TraceExtent.Z);

    TArray<FHitResult> HitResults;
    TArray<AActor*> ActorsToIgnore;
    ActorsToIgnore.Add(this);
    ActorsToIgnore.Add(GetOwner());

    bool bHit = UKismetSystemLibrary::BoxTraceMulti(
        this,
        LastCenterLocation,
        CurrentCenterLocation,
        BoxHalfSize,
        BoxRotation,
        UEngineTypes::ConvertToTraceType(ECC_Pawn),
        false,
        ActorsToIgnore,
        EDrawDebugTrace::ForDuration,
        HitResults,
        true
    );

    if (bHit)
    {
        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            if (HitActor && !HitActors.Contains(HitActor))
            {
                HitActors.Add(HitActor);

                APawn* OwnerPawn = Cast<APawn>(GetOwner());
                if (OwnerPawn)
                {
                    UGameplayStatics::ApplyDamage(
                        HitActor,
                        Damage,
                        OwnerPawn->GetController(),
                        this,
                        UDamageType::StaticClass()
                    );
                }
            }
        }
    }
        
    LastCenterLocation = CurrentCenterLocation;
}

void AWeapon_Item_Base::OnAttackHit(
    UPrimitiveComponent* OverlappedComponent,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult& SweepResult
)
{
    if (OtherActor == GetOwner())
    {
        return;
    }

    APawn* OwnerPawn = Cast<APawn>(GetOwner());
    if (OwnerPawn)
    {
        UGameplayStatics::ApplyDamage(
            OtherActor,
            Damage,
            OwnerPawn->GetController(),
            this,
            UDamageType::StaticClass()
        );
    }
}
