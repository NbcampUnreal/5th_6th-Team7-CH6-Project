// Sword_Item.cpp

#include "Item/Sword_Item.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ASword_Item::ASword_Item()
{

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
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	APawn* HitPawn = Cast<APawn>(OtherActor);
	if (!HitPawn)
	{
		return;
	}

	const float Damage = 20.f;

	UGameplayStatics::ApplyDamage(HitPawn,Damage,GetInstigatorController(),this,UDamageType::StaticClass());

	UE_LOG(LogTemp, Log, TEXT("[Sword] Hit %s (%f Damage)"), *OtherActor->GetName(), Damage);
}
