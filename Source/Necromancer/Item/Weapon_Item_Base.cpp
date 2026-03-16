//Weapon_Item_Base.cpp

#include "Weapon_Item_Base.h"

#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Sword_Item.h"
#include "Kismet/KismetMathLibrary.h"
#include "Character/NecPlayerCharacter.h"
#include "Engine/AssetManager.h"
#include "DamageType/NecDamageType.h"

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

    if (WeaponData && !WeaponData->SwingSound.IsNull())
    {
        USoundBase* SoundToPlay = WeaponData->SwingSound.Get();
        if (!SoundToPlay && WeaponData->SwingSound.IsPending())
        {
            SoundToPlay = WeaponData->SwingSound.LoadSynchronous();
        }

        if (SoundToPlay)
        {
            UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, GetActorLocation());
        }
    }
}

void AWeapon_Item_Base::EndAttack()
{
    bIsAttacking = false;
    SetActorTickEnabled(false);
}

UAnimMontage* AWeapon_Item_Base::GetAttackMontage() const
{
    if (WeaponData && WeaponData->AttackMontage.IsValid())
    {
        return WeaponData->AttackMontage.Get();
    }

    return WeaponData ? WeaponData->AttackMontage.LoadSynchronous() : nullptr;
}

UAnimMontage* AWeapon_Item_Base::GetGuardMontage() const
{
    if (WeaponData && WeaponData->GuardMontage.IsValid())
    {
        return WeaponData->GuardMontage.Get();
    }

    return WeaponData ? WeaponData->GuardMontage.LoadSynchronous() : nullptr;
}

void AWeapon_Item_Base::PreloadWeaponAssets()
{
    if (!WeaponData)
    {
        return;
    }

    TArray<FSoftObjectPath> AssetsToLoad;

    if (!WeaponData->AttackMontage.IsNull())
    {
        AssetsToLoad.AddUnique(WeaponData->AttackMontage.ToSoftObjectPath());
    }

    if (!WeaponData->GuardMontage.IsNull())
    {
        AssetsToLoad.AddUnique(WeaponData->GuardMontage.ToSoftObjectPath());
    }

    if (!WeaponData->SwingSound.IsNull())
    {
        AssetsToLoad.AddUnique(WeaponData->SwingSound.ToSoftObjectPath());
    }

    if (!WeaponData->AttackSound.IsNull())
    {
        AssetsToLoad.AddUnique(WeaponData->AttackSound.ToSoftObjectPath());
    }

    if (AssetsToLoad.Num() > 0)
    {
        FStreamableManager& StreamableManager = UAssetManager::GetStreamableManager();
        AssetLoadHandle = StreamableManager.RequestAsyncLoad(
            AssetsToLoad,
            FStreamableDelegate::CreateUObject(this, &AWeapon_Item_Base::OnWeaponAssetsLoaded)
        );
    }
}

void AWeapon_Item_Base::Equip(AActor* Equip_Owner)
{
    SetReplicates(true);

    ACharacter* Character = Cast<ACharacter>(Equip_Owner);
    if (Character && Character->GetMesh())
    {
        FName SocketName = "hand_r_weapon";

        AttachToComponent(
            Character->GetMesh(),
            FAttachmentTransformRules::SnapToTargetNotIncludingScale,
            SocketName
        );

    }
}

void AWeapon_Item_Base::Multicast_PlayHitSound_Implementation(FVector HitLocation)
{
    if (WeaponData && !WeaponData->AttackSound.IsNull())
    {
        USoundBase* SoundToPlay = WeaponData->AttackSound.Get();
        if (!SoundToPlay && WeaponData->AttackSound.IsPending())
        {
            SoundToPlay = WeaponData->AttackSound.LoadSynchronous();
        }

        if (SoundToPlay)
        {
            UGameplayStatics::PlaySoundAtLocation(this, SoundToPlay, HitLocation);
        }
    }
}

void AWeapon_Item_Base::PerformTrace()
{
    USkeletalMeshComponent* TraceMesh = WeaponMesh;

    if (bIsUnarmed)
    {
        if (APawn* OwnerPawn = Cast<APawn>(GetOwner()))
        {
            if (ANecPlayerCharacter* OwnerChar = Cast<ANecPlayerCharacter>(OwnerPawn))
            {
                TraceMesh = OwnerChar->GetMesh();
            }
        }       
    }

    if (!TraceMesh)
    {
        return;
    }

    const FVector StartPos = TraceMesh->GetSocketLocation(StartSocketName);
    const FVector EndPos = TraceMesh->GetSocketLocation(EndSocketName);
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

    FCollisionObjectQueryParams ObjectQueryParams;
    ObjectQueryParams.AddObjectTypesToQuery(ECC_Pawn);

    TArray<FHitResult> HitResults;
    bool bHit = GetWorld()->SweepMultiByObjectType(
        HitResults,
        LastCenterLocation,
        CurrentCenterLocation,
        BoxRotation,
        ObjectQueryParams,
        BoxShape,
        QueryParams
    );

    if (bDrawDebug)
    {
        FColor DrawColor = bHit ? FColor::Green : FColor::Red;
        DrawDebugBox(GetWorld(), LastCenterLocation, BoxHalfExtent, BoxRotation, DrawColor, false, 1.0f);
        DrawDebugLine(GetWorld(), LastCenterLocation, CurrentCenterLocation, FColor::Yellow, false, 1.0f);
    }

    if (bHit)
    {
        APawn* OwnerPawn = Cast<APawn>(GetOwner());
        if (!OwnerPawn)
        {
            return;
        }

        IGenericTeamAgentInterface* AttackerTeam = Cast<IGenericTeamAgentInterface>(OwnerPawn);

        for (const FHitResult& Hit : HitResults)
        {
            AActor* HitActor = Hit.GetActor();
            if (HitActor && !HitActors.Contains(HitActor))
            {
                IGenericTeamAgentInterface* VictimTeam = Cast<IGenericTeamAgentInterface>(HitActor);
                if (!VictimTeam)
                {
                    continue;
                }

                if (AttackerTeam && VictimTeam)
                {
                    if (AttackerTeam->GetGenericTeamId() == VictimTeam->GetGenericTeamId())
                    {
                        continue;
                    }
                }

                HitActors.Add(HitActor);

                float FinalDamage = WeaponData ? WeaponData->BaseDamage * CurrentDamageMultiplier : 0;

                UGameplayStatics::ApplyDamage(
                    HitActor,
                    FinalDamage,
                    OwnerPawn->GetController(),
                    this,
                    UNecDamageType::StaticClass()
                );

                FVector HitLocation = Hit.ImpactPoint;
                Multicast_PlayHitSound(HitLocation);
            }
        }
    }

    LastCenterLocation = CurrentCenterLocation;
}

void AWeapon_Item_Base::OnWeaponAssetsLoaded()
{
    UE_LOG(LogTemp, Log, TEXT("Weapon assets loaded successfully."));

    if (AssetLoadHandle.IsValid())
    {
        AssetLoadHandle->ReleaseHandle();
        AssetLoadHandle.Reset();
    }
}