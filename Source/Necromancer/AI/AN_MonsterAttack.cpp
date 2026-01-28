// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_MonsterAttack.h"
#include "MonsterStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "DrawDebugHelpers.h"

void UAN_MonsterAttack::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}
	UMonsterStatComponent* MonsterStatComponent = OwnerActor->FindComponentByClass<UMonsterStatComponent>();
	if (!MonsterStatComponent)
	{
		return;
	}
	FVector AttackStart = OwnerActor->GetActorLocation();
	FVector AttackEnd = AttackStart + OwnerActor->GetActorForwardVector() * AttackDistance;
    
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerActor);
    
	bool bHit = OwnerActor->GetWorld()->SweepMultiByChannel(HitResults,AttackStart,AttackEnd,FQuat::Identity,ECC_Pawn,FCollisionShape::MakeSphere(AttackRadius),Params);
    #if ENABLE_DRAW_DEBUG
    UWorld* World = OwnerActor->GetWorld();
    if (World)
    {
        float DebugDuration = 2.0f;  // 2초간 표시
        
        // 시작점 구체 (파란색)
        DrawDebugSphere(World, AttackStart, AttackRadius, 12, FColor::Blue, false, DebugDuration);
        
        // 끝점 구체 (히트 여부에 따라 색상 변경)
        FColor EndColor = bHit ? FColor::Red : FColor::Green;
        DrawDebugSphere(World, AttackEnd, AttackRadius, 12, EndColor, false, DebugDuration);
        
        // 시작점에서 끝점까지 라인
        DrawDebugLine(World, AttackStart, AttackEnd, FColor::Yellow, false, DebugDuration, 0, 2.0f);
        
        // 캡슐 형태로 전체 스윕 영역 표시 (선택사항)
        DrawDebugCapsule(
            World,
            (AttackStart + AttackEnd) / 2,           // 중심점
            AttackDistance / 2,                       // 절반 높이
            AttackRadius,                             // 반지름
            FRotationMatrix::MakeFromZ(OwnerActor->GetActorForwardVector()).ToQuat(),
            FColor::Orange,
            false,
            DebugDuration
        );
        
        // 히트된 액터들 표시
        for (const FHitResult& Hit : HitResults)
        {
            // 히트 지점에 빨간 점
            DrawDebugPoint(World, Hit.ImpactPoint, 15.0f, FColor::Red, false, DebugDuration);
            
            // 히트된 액터 위치에 박스
            if (Hit.GetActor())
            {
                DrawDebugBox(
                    World,
                    Hit.GetActor()->GetActorLocation(),
                    FVector(30, 30, 30),
                    FColor::Red,
                    false,
                    DebugDuration
                );
                
                // 히트된 액터 이름 표시
                DrawDebugString(
                    World,
                    Hit.GetActor()->GetActorLocation() + FVector(0, 0, 100),
                    Hit.GetActor()->GetName(),
                    nullptr,
                    FColor::White,
                    DebugDuration
                );
            }
        }
    }
#endif
    // ========== 디버그 드로잉 끝 ==========
	for (const FHitResult& Hit : HitResults)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			UGameplayStatics::ApplyDamage(HitActor,MonsterStatComponent->GetAttackPower(),OwnerActor->GetInstigatorController(),OwnerActor,nullptr);
		}
	}

	
}
