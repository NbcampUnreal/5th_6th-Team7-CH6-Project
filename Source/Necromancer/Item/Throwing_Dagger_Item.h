//Throwing_Dagger_Item,h

#pragma once

#include "CoreMinimal.h"
#include "Item/Throwing_Item.h"
#include "Throwing_Dagger_Item.generated.h"

UCLASS()
class NECROMANCER_API AThrowing_Dagger_Item : public AThrowing_Item
{
	GENERATED_BODY()

public:
	AThrowing_Dagger_Item();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dagger")
	float Damage = 25.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dagger")
	bool bStickToTarget = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dagger")
	float LifeAfterHit = 5.f;

protected:
	virtual void OnHit(
		UPrimitiveComponent* HitComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		FVector NormalImpulse,
		const FHitResult& Hit
	) override;

private:
	void HandleDamage(AActor* OtherActor);

	void HandleStick(UPrimitiveComponent* OtherComp, const FHitResult& Hit);
};