#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_AssignCombatRole.generated.h"

// 슬롯/거리 기반으로 전투 역할(Attacker/Flanker/Waiter) 할당
UCLASS()
class NECROMANCER_API UBTService_AssignCombatRole : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_AssignCombatRole();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual FString GetStaticDescription() const override;

	// Flanker/Waiter 구분 거리
	UPROPERTY(EditAnywhere, Category = "Role")
	float FlankerMaxDistance = 500.0f;
};
