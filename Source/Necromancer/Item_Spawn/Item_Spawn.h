//Item_Spawn,h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Item_Spawn.generated.h"

class UBoxComponent;

UCLASS()
class NECROMANCER_API AItem_Spawn : public AActor
{
	GENERATED_BODY()
	
public:	
	AItem_Spawn();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawnable Items")
	USceneComponent* SceneComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Spawnable Items")
	UBoxComponent* SpawnBoxComp;

	UFUNCTION(BlueprintCallable, Category = "Spawn Items")
	FVector GetRandomPointInVolume() const;
	UFUNCTION(BlueprintCallable, Category = "Spawn Items")
	void SpawnItem(TSubclassOf<AActor> ItemClass);
};
