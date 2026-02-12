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
    WeaponMesh->SetupAttachment(RootComponent);
    WeaponMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;

    TracePreviewBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TracePreviewBox"));
    TracePreviewBox->SetupAttachment(WeaponMesh);
    TracePreviewBox->SetHiddenInGame(true);
    TracePreviewBox->SetCollisionProfileName(TEXT("NoCollision"));
    TracePreviewBox->ShapeColor = FColor::Red;
    TracePreviewBox->SetLineThickness(1.0f);
}

void AWeapon_Item_Base::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsAttacking)
    {
        PerformTrace();
    }
}

void AWeapon_Item_Base::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    if (WeaponMesh && WeaponMesh->DoesSocketExist(StartSocketName) && WeaponMesh->DoesSocketExist(EndSocketName))
    {
        const FVector StartPos = WeaponMesh->GetSocketLocation(StartSocketName);
        const FVector EndPos = WeaponMesh->GetSocketLocation(EndSocketName);
        const FVector CenterPos = (StartPos + EndPos) * 0.5f;
        const FVector Direction = EndPos - StartPos;
        const float WeaponLength = Direction.Length();

        if (!Direction.IsNearlyZero())
        {
            FQuat BoxRotation = FRotationMatrix::MakeFromX(Direction).ToQuat();

            TracePreviewBox->SetWorldLocation(CenterPos);
            TracePreviewBox->SetWorldRotation(BoxRotation);
            TracePreviewBox->SetBoxExtent(FVector(WeaponLength * 0.5f, TraceExtent.Y, TraceExtent.Z));
        }
    }
}

void AWeapon_Item_Base::StartAttack()
{
    bIsAttacking = true;
    HitActors.Empty();

    FVector StartPos = WeaponMesh->GetSocketLocation(StartSocketName);
    FVector EndPos = WeaponMesh->GetSocketLocation(EndSocketName);

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
    const FVector StartPos = WeaponMesh->GetSocketLocation(StartSocketName);
    const FVector EndPos = WeaponMesh->GetSocketLocation(EndSocketName);
    const FVector CurrentCenterLocation = (StartPos + EndPos) * 0.5f;
    const FVector Direction = EndPos - StartPos;
    const float WeaponLength = Direction.Length();

    const FQuat BoxRotation = FRotationMatrix::MakeFromX(Direction).ToQuat();

    const FVector BoxHalfExtent = FVector(WeaponLength * 0.5f, TraceExtent.Y, TraceExtent.Z);
    const FCollisionShape BoxShape = FCollisionShape::MakeBox(BoxHalfExtent);

    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    QueryParams.AddIgnoredActor(GetOwner());
    QueryParams.bTraceComplex = false;
    QueryParams.bReturnPhysicalMaterial = false;

    TArray<FHitResult> HitResults;
    bool bHit = GetWorld()->SweepMultiByChannel(
        HitResults,
        LastCenterLocation,
        CurrentCenterLocation,
        BoxRotation,
        ECC_Pawn,
        BoxShape,
        QueryParams
    );

    FColor DrawColor = bHit ? FColor::Green : FColor::Red;
    DrawDebugBox(GetWorld(), LastCenterLocation, BoxHalfExtent, BoxRotation, DrawColor, false, 1.0f);
    DrawDebugLine(GetWorld(), LastCenterLocation, CurrentCenterLocation, FColor::Yellow, false, 1.0f);

    if (bHit)
    {
        APawn* OwnerPawn = Cast<APawn>(GetOwner());
        if (!OwnerPawn)
        {
            return;
        }

        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            if (HitActor && !HitActors.Contains(HitActor))
            {
                HitActors.Add(HitActor);

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

}