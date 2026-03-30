#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingComponent.generated.h"

class ANecPlayerCharacter;
class UUserWidget;

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

	void HandleLockOnInput(FVector2D LookInput);

protected:
	UFUNCTION(Server, Reliable)
	void Server_SwitchTarget(bool bIsRight);

	UFUNCTION(Server, Reliable)
	void Server_ClearLockOn();

	UFUNCTION(Server, Reliable)
	void Server_SetLockOnTarget(AActor* NewTarget);

	UFUNCTION()
	void OnRep_CurrentTarget();

	void FindTarget();
	void ClearLockOn();
	void InitLockOnWidget();
	void UpdateLockOnUI();

protected:
	UPROPERTY(ReplicatedUsing = OnRep_CurrentTarget)
	TObjectPtr<AActor> CurrentTarget;

	UPROPERTY()
	TObjectPtr<ANecPlayerCharacter> OwnerCharacter;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> LockOnWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> LockOnWidgetInstance;

	UPROPERTY(EditAnywhere, Category = "Targeting")
	FName TargetSocketName = FName("spine_05");

	UPROPERTY(EditAnywhere, Category = "Targeting")
	float SearchRadius = 1000.0f;

	UPROPERTY(EditAnywhere, Category = "Targeting")
	float LockOnPitch = -25.0f;

	UPROPERTY(EditAnywhere, Category = "Targeting")
	float SwitchCooldown = 0.25f;

	UPROPERTY(EditAnywhere, Category = "Targeting")
	float SwitchThreshold = 5.0f;

	float LastSwitchTime = 0.0f;
};