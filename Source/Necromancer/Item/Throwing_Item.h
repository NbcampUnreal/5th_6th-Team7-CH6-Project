//Throwing_Item.h

#pragma once

#include "CoreMinimal.h"
#include "Item/Item_Consumption_Bass.h"
#include "Throwing_Item.generated.h"

//class USphereComponent;
//class UProjectileMovementComponent;

UCLASS()
class NECROMANCER_API AThrowing_Item : public AItem_Consumption_Bass
{
	GENERATED_BODY()

//public:
//	AThrowing_Item();
//
//	virtual void ExecuteUse(ACharacter* User) override;

//protected:
//	UFUNCTION(Server, Reliable)
//	void Server_ExecuteUse(ACharacter* User);
//
//	UFUNCTION(NetMulticast, Reliable)
//	void Multicast_OnHitFX(FVector Location);
//
//	UFUNCTION()
//	virtual void OnHit(
//		UPrimitiveComponent* HitComp,
//		AActor* OtherActor,
//		UPrimitiveComponent* OtherComp,
//		FVector NormalImpulse,
//		const FHitResult& Hit);

//protected:
//	UPROPERTY(VisibleAnywhere)
//	USphereComponent* Collision;
//
//	UPROPERTY(VisibleAnywhere)
//	UProjectileMovementComponent* ProjectileMovement;
//
//	UPROPERTY(EditDefaultsOnly, Category = "Throw")
//	float ThrowSpeed = 1500.f;
};
