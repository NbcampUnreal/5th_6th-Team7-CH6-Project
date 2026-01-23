#include "GAS/Monster/MonsterBase.h"
#include "GAS/Monster/MonsterAttributeSet.h"
#include "GameFramework/CharacterMovementComponent.h"

AMonsterBase::AMonsterBase()
{
	MonsterAttributeSet = CreateDefaultSubobject<UMonsterAttributeSet>(TEXT("MonsterAttributeSet"));

	// 이동 방향으로 회전 설정
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 480.f, 0.f);
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
}

void AMonsterBase::BeginPlay()
{
	Super::BeginPlay();
}

void AMonsterBase::GetActorEyesViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	// 메쉬 기준 눈 위치 (머리 높이)
	OutLocation = GetActorLocation() + FVector(0.f, 0.f, BaseEyeHeight);

	// 액터의 현재 회전 사용 (Controller가 아닌 Pawn 회전)
	OutRotation = GetActorRotation();
}
