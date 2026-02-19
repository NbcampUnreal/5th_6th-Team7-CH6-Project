// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldActor/InteractableActor.h"
#include "SubmitBusket.generated.h"

/**
 * 
 */
class UBucketInventoryComponent;
class UNecInventoryComponent;


UCLASS()
class NECROMANCER_API ASubmitBusket : public AInteractableActor
{
	GENERATED_BODY()
private:
	
public:
	ASubmitBusket();
	virtual void BeginPlay()override;
	virtual void Interact_Implementation(AActor* Interactor) override;
	UFUNCTION()
	void Submit();	
private:
	UFUNCTION(Server, Reliable)
	void Server_Submit();
	void Submit_Internal();
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	UBucketInventoryComponent* BucketInventory;
	UPROPERTY(ReplicatedUsing = OnRep_IsActive, BlueprintReadOnly, Category = "Submit")
	bool bIsActive;
protected:
	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps)
		const override;
	UFUNCTION()
	void OnRep_IsActive();

	virtual void OnSphereEnd(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
};
