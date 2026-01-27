#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "NecPlayerState.generated.h"

class UStatComponent;
class UStaminaComponent;

UCLASS()
class NECROMANCER_API ANecPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:
	ANecPlayerState();

	UStatComponent* GetStatComponent() const { return StatComponent; }
	UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }

protected:
	virtual void CopyProperties(APlayerState* PlayerState) override;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStatComponent> StatComponent;

	UPROPERTY(VisibleAnywhere, Category = "Component")
	TObjectPtr<UStaminaComponent> StaminaComponent;
};