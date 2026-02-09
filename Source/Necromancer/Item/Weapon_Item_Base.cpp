//Weapon_Item_Base.cpp

#include "Weapon_Item_Base.h"

#include "Components/BoxComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sword_Item.h"

AWeapon_Item_Base::AWeapon_Item_Base()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	AttackHitBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AttackHitBox"));
	AttackHitBox->SetupAttachment(WeaponMesh);

	AttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	AttackHitBox->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttackHitBox->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	AttackHitBox->OnComponentBeginOverlap.AddDynamic(this, &AWeapon_Item_Base::OnAttackHit);
}

void AWeapon_Item_Base::StartAttack()
{
	AttackHitBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);

	if (AttackSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this,AttackSound,GetActorLocation());
	}
}

void AWeapon_Item_Base::EndAttack()
{
	AttackHitBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
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
