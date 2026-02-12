//Mace_Item.h

#pragma once

#include "CoreMinimal.h"
#include "Item/Weapon_Item_Base.h"
#include "Mace_Item.generated.h"

UCLASS()
class NECROMANCER_API AMace_Item : public AWeapon_Item_Base
{
	GENERATED_BODY()

public:
	AMace_Item();

protected:
	virtual void OnAttackHit(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	) override;
};
