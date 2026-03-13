#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_BossRoomPatrol.generated.h"

class ABossMonsterBase;

// 보스 방 순회 순찰 Task
// NecDungeonsGenerator에서 생성된 방 위치를 가져와 셔플 후 순서대로 방문
UCLASS()
class NECROMANCER_API UBTTask_BossRoomPatrol : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_BossRoomPatrol();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

private:
	// 방 목록 초기화 (Generator에서 가져와 셔플)
	bool InitializeRoomLocations(UWorld* World);

	// 셔플 (Fisher-Yates)
	void ShuffleRooms();

	// 방 목록 캐시 (셔플된 상태)
	TArray<FVector> ShuffledRoomLocations;

	// 현재 방문 중인 방 인덱스
	int32 CurrentRoomIndex = 0;

	// 방 목록 초기화 여부
	bool bRoomsInitialized = false;
};
