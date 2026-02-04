//Sword_Item.h

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon_Item_Base.h"
#include "Sword_Item.generated.h"

UCLASS()
class NECROMANCER_API ASword_Item : public AWeapon_Item_Base
{
	GENERATED_BODY()
	
public:
	ASword_Item();

protected:
	virtual void OnAttackHit(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);
};
