#include "BTTask_MonsterBlock.h"
#include "AIController.h"
#include "MonsterBase.h"

UBTTask_MonsterBlock::UBTTask_MonsterBlock()
{
	NodeName = "Monster Block";
}

EBTNodeResult::Type UBTTask_MonsterBlock::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC)
	{
		return EBTNodeResult::Failed;
	}

	AMonsterBase* Monster = Cast<AMonsterBase>(AIC->GetPawn());
	if (!Monster)
	{
		return EBTNodeResult::Failed;
	}

	Monster->SetBlockingState(bEnableBlock);
	return EBTNodeResult::Succeeded;
}

FString UBTTask_MonsterBlock::GetStaticDescription() const
{
	return FString::Printf(TEXT("Block: %s"), bEnableBlock ? TEXT("ON") : TEXT("OFF"));
}
