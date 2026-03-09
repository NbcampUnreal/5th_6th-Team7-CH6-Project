#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NecPlayerState.generated.h"

class UStatComponent;
class UStaminaComponent;
class UNecInventoryComponent;
class USoulComponent;

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
};