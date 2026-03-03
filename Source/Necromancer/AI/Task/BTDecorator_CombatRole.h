#pragma once

#include "CoreMinimal.h"
#include "Necromancer.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CombatRole.generated.h"

// BB의 CombatRole 값이 지정한 역할과 일치하는지 체크
UCLASS()
class NECROMANCER_API UBTDecorator_CombatRole : public UBTDecorator
{
	GENERATED_BODY()

public:
	UBTDecorator_CombatRole();

protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	virtual FString GetStaticDescription() const override;

	UPROPERTY(EditAnywhere, Category = "Role")
	ECombatRole RequiredRole = ECombatRole::Attacker;
};
