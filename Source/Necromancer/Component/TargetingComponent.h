#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingComponent.generated.h"

class ANecPlayerCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NECROMANCER_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:		
	UTargetingComponent();

protected:	
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	void ToggleLockOn();
	AActor* GetCurrentTarget() const { return CurrentTarget; }

protected:
	void FindTarget();
	void ClearLockOn();

protected:
	UPROPERTY(Replicated)
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY()
	TObjectPtr<ANecPlayerCharacter> OwnerCharacter;

	UPROPERTY(EditAnywhere, Category = "Targeting")
	float SearchRadius = 1000.0f;
};