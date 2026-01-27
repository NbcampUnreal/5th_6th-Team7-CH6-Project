// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GridInventoryComponent.generated.h"

class UItemInstance;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NECROMANCER_API UGridInventoryComponent : public UActorComponent
{
	GENERATED_BODY()
public:
    // Sets default values for this component's properties
    UGridInventoryComponent();
protected:
    // Called when the game starts
    virtual void BeginPlay() override;

    virtual void GetLifetimeReplicatedProps(
        TArray<FLifetimeProperty>& OutLifetimeProps
    ) const override;

    virtual bool ReplicateSubobjects(
        class UActorChannel* Channel,
        class FOutBunch* Bunch,
        FReplicationFlags* RepFlags
    ) override;

private:
    UPROPERTY(Replicated)
    TArray<FGuid> RootItemGuid;
    UPROPERTY(ReplicatedUsing = OnRep_Items)
    TArray<UItemInstance*> Items;

    TMap<FGuid, TArray<UItemInstance*>> ItemsByOwnerGuid;
public:

private:
    void RebuildItemOwnerMap();
public:
    UFUNCTION()
    void OnRep_Items();
		
};
