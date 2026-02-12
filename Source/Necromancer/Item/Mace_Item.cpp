// Mace_Item.cpp

#include "Item/Mace_Item.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

AMace_Item::AMace_Item()
{
	Damage = 20.f;

	//// 메쉬
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> MaceMeshAsset(TEXT(""));
	//if (MaceMeshAsset.Succeeded() && WeaponMesh)
	//{
	//	WeaponMesh->SetSkeletalMesh(MaceMeshAsset.Object);
	//}

	//// 사운드
	//static ConstructorHelpers::FObjectFinder<USoundBase> MaceAttackSound(TEXT(""));
	//if (MaceAttackSound.Succeeded())
	//{
	//	AttackSound = MaceAttackSound.Object;
	//}
}

void AMace_Item::OnAttackHit(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult
)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	APawn* HitPawn = Cast<APawn>(OtherActor);
	if (!HitPawn)
	{
		return;
	}

	UGameplayStatics::ApplyDamage(
		HitPawn,
		Damage,
		GetInstigatorController(),
		this,
		UDamageType::StaticClass()
	);

	UE_LOG(LogTemp, Log, TEXT("[Mace] Hit %s (%f Damage)"), *OtherActor->GetName(), Damage);
}
