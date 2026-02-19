// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WorldActor/InteractableActor.h"

#include "DropItemBase.generated.h"

class UItemInstanceComponent;
class UNecInventoryComponent;
UCLASS()
class NECROMANCER_API ADropItemBase : public AInteractableActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ADropItemBase();
	virtual void BeginPlay()override;
	UItemInstanceComponent* GetItemInstanceComponent() const
	{
		return ItemInstanceComponent;
	}
	virtual void Interact_Implementation(AActor* Interactor) override;

private:
	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* Interactor);
	virtual void Interact_Internal(AActor* Interactor)override;
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item")
	UItemInstanceComponent* ItemInstanceComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item")
	FName ItemID = TEXT("0_000");
};
