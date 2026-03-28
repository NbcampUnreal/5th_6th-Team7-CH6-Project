#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NecPlayerState.generated.h"

class UStatComponent;
class UStaminaComponent;
class UNecInventoryComponent;
class USoulComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGraceTimeChanged, int32, NewTime);

UCLASS()
class NECROMANCER_API ANecPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ANecPlayerState();

	UStatComponent* GetStatComponent() const { return StatComponent; }
	UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	UNecInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	USoulComponent* GetSoulComponent() const { return SoulComponent; }
protected:
	virtual void CopyProperties(APlayerState* PlayerState) override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStatComponent> StatComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaminaComponent> StaminaComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UNecInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<USoulComponent> SoulComponent;

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(ReplicatedUsing = OnRep_GraceTimeForRevive)
	int32 GraceTimeForRevive = -1;

	UFUNCTION()
	void OnRep_GraceTimeForRevive();

	UPROPERTY()
	FOnGraceTimeChanged OnGraceTimeChanged;
};