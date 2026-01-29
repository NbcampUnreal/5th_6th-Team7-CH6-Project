// Fill out your copyright notice in the Description page of Project Settings.


#include "AN_MonsterAttack.h"
#include "MonsterStatComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GenericTeamAgentInterface.h"
#include "DrawDebugHelpers.h"
#include "Necromancer.h"

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
        
        
        DrawDebugSphere(World, AttackStart, AttackRadius, 12, FColor::Blue, false, DebugDuration);
        
        
        FColor EndColor = bHit ? FColor::Red : FColor::Green;
        DrawDebugSphere(World, AttackEnd, AttackRadius, 12, EndColor, false, DebugDuration);
        
        
        DrawDebugLine(World, AttackStart, AttackEnd, FColor::Yellow, false, DebugDuration, 0, 2.0f);
        
        
        DrawDebugCapsule(
            World,
            (AttackStart + AttackEnd) / 2,           
            AttackDistance / 2,                       
            AttackRadius,                             
            FRotationMatrix::MakeFromZ(OwnerActor->GetActorForwardVector()).ToQuat(),
            FColor::Orange,
            false,
            DebugDuration
        );
        
        
        for (const FHitResult& Hit : HitResults)
        {
            
            DrawDebugPoint(World, Hit.ImpactPoint, 15.0f, FColor::Red, false, DebugDuration);
            
          
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
			IGenericTeamAgentInterface* GenericTeamAgentInterface = Cast<IGenericTeamAgentInterface>(HitActor);
			if (GenericTeamAgentInterface && GenericTeamAgentInterface -> GetGenericTeamId() == FGenericTeamId(TEAM_ID_MONSTER))
			{
				continue;
			}
			UGameplayStatics::ApplyDamage(HitActor,MonsterStatComponent->GetAttackPower(),OwnerActor->GetInstigatorController(),OwnerActor,nullptr);
		}
	}

	
}
