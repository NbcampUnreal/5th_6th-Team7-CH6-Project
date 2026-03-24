#include "BTTask_BossRoomPatrol.h"
#include "BossMonsterBase.h"
#include "Necromancer.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Maps/NecDungeonsGenerator.h"

UBTTask_BossRoomPatrol::UBTTask_BossRoomPatrol()
{
	NodeName = TEXT("Boss Room Patrol");
	bCreateNodeInstance = true;
}

EBTNodeResult::Type UBTTask_BossRoomPatrol::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIC = OwnerComp.GetAIOwner();
	if (!AIC || !AIC->GetPawn())
	{
		return EBTNodeResult::Failed;
	}

	ABossMonsterBase* Boss = Cast<ABossMonsterBase>(AIC->GetPawn());
	if (!Boss)
	{
		return EBTNodeResult::Failed;
	}

	UBlackboardComponent* BB = OwnerComp.GetBlackboardComponent();
	if (!BB)
	{
		return EBTNodeResult::Failed;
	}

	// 순찰 비활성화 상태면 실패 반환 (BT에서 Wait 브랜치로 진입)
	if (!Boss->IsPatrolEnabled())
	{
		return EBTNodeResult::Failed;
	}

	// 방 목록 초기화 (최초 1회)
	if (!bRoomsInitialized)
	{
		if (!InitializeRoomLocations(Boss->GetWorld()))
		{
			return EBTNodeResult::Failed;
		}
	}

	// 방 목록이 비어있으면 실패
	if (ShuffledRoomLocations.Num() == 0)
	{
		return EBTNodeResult::Failed;
	}

	// 모든 방을 방문했으면 재셔플
	if (CurrentRoomIndex >= ShuffledRoomLocations.Num())
	{
		ShuffleRooms();
		CurrentRoomIndex = 0;
		UE_LOG(LogMonsterAI, Log, TEXT("[BossRoomPatrol] All rooms visited, reshuffling. Total rooms: %d"), ShuffledRoomLocations.Num());
	}

	// 현재 인덱스의 방 위치 가져오기
	FVector RoomLocation = ShuffledRoomLocations[CurrentRoomIndex];

	// NavMesh 위 위치로 보정
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	if (NavSys)
	{
		FNavLocation NavLocation;
		if (NavSys->ProjectPointToNavigation(RoomLocation, NavLocation, FVector(500.0f, 500.0f, 500.0f)))
		{
			RoomLocation = NavLocation.Location;
		}
	}

	// 순찰 이동 속도 적용
	if (UCharacterMovementComponent* MoveComp = Boss->GetCharacterMovement())
	{
		MoveComp->MaxWalkSpeed = Boss->GetPatrolWalkSpeed();
	}

	// 순찰 모드 이동 (이동 방향으로 회전)
	Boss->SetCombatMovementMode(false);

	// BB에 이동 목표 위치 설정
	BB->SetValueAsVector(NAME_PatrolLocation, RoomLocation);

	UE_LOG(LogMonsterAI, Log, TEXT("[BossRoomPatrol] Moving to room %d/%d at location: %s"),
		CurrentRoomIndex + 1, ShuffledRoomLocations.Num(), *RoomLocation.ToString());

	// 다음 방으로 인덱스 진행
	CurrentRoomIndex++;

	return EBTNodeResult::Succeeded;
}

bool UBTTask_BossRoomPatrol::InitializeRoomLocations(UWorld* World)
{
	if (!World)
	{
		return false;
	}

	// NecDungeonsGenerator 찾기
	ANecDungeonsGenerator* Generator = Cast<ANecDungeonsGenerator>(
		UGameplayStatics::GetActorOfClass(World, ANecDungeonsGenerator::StaticClass()));

	if (!Generator)
	{
		UE_LOG(LogMonsterAI, Warning, TEXT("[BossRoomPatrol] NecDungeonsGenerator not found!"));
		return false;
	}

	// 던전 생성 완료 확인
	if (!Generator->GetbIsDungeonComplete())
	{
		UE_LOG(LogMonsterAI, Warning, TEXT("[BossRoomPatrol] Dungeon generation not complete yet!"));
		return false;
	}

	// 방 위치 복사
	const TArray<FVector>& RoomLocs = Generator->GetRoomLocations();
	if (RoomLocs.Num() == 0)
	{
		UE_LOG(LogMonsterAI, Warning, TEXT("[BossRoomPatrol] No room locations available!"));
		return false;
	}

	ShuffledRoomLocations = RoomLocs;
	ShuffleRooms();
	CurrentRoomIndex = 0;
	bRoomsInitialized = true;

	UE_LOG(LogMonsterAI, Log, TEXT("[BossRoomPatrol] Initialized with %d rooms"), ShuffledRoomLocations.Num());
	return true;
}

void UBTTask_BossRoomPatrol::ShuffleRooms()
{
	// Fisher-Yates 셔플
	for (int32 i = ShuffledRoomLocations.Num() - 1; i > 0; --i)
	{
		int32 j = FMath::RandRange(0, i);
		ShuffledRoomLocations.Swap(i, j);
	}
}
