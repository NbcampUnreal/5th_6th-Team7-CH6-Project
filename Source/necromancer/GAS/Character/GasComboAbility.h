#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "GasComboAbility.generated.h"

UCLASS()
class NECROMANCER_API UGasComboAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGasComboAbility();

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Combo")
	int32 CurrentComboIndex;

	UPROPERTY(BlueprintReadWrite, Category = "Combo")
	int32 MaxComboCount;

	UPROPERTY(BlueprintReadWrite, Category = "Combo")
	bool bNextComboInput;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Combo")
	UAnimMontage* ComboMontage;

	UFUNCTION(BlueprintPure, Category = "Combo")
	FName GetNextSectionName() const;

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ResetCombo();

	UFUNCTION(BlueprintCallable, Category = "Combo")
	void CommitComboCost();
};
