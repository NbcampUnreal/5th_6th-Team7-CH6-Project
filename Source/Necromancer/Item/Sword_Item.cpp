// Sword_Item.cpp

#include "Item/Sword_Item.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ASword_Item::ASword_Item()
{
	Damage = 20.f;

	////매쉬
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> SwordMeshAsset(	TEXT(""));

	//if (SwordMeshAsset.Succeeded() && WeaponMesh)
	//{
	//	WeaponMesh->SetSkeletalMesh(SwordMeshAsset.Object);
	//}

	////사운드
	//static ConstructorHelpers::FObjectFinder<USoundBase> SwordAttackSound(TEXT(""));

	//if (SwordAttackSound.Succeeded())
	//{
	//	AttackSound = SwordAttackSound.Object;
	//}
}

void ASword_Item::OnAttackHit(
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

	UGameplayStatics::ApplyDamage(HitPawn,Damage,GetInstigatorController(),this,UDamageType::StaticClass());

	UE_LOG(LogTemp, Log, TEXT("[Sword] Hit %s (%f Damage)"), *OtherActor->GetName(), Damage);
}
