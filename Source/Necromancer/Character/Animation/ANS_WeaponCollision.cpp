#include "Character/Animation/ANS_WeaponCollision.h"
#include "Character/NecPlayerCharacter.h"
#include "Component/CombatComponent.h"

void UANS_WeaponCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		ANecPlayerCharacter* Character = Cast<ANecPlayerCharacter>(MeshComp->GetOwner());
		if (Character)
		{
			UCombatComponent* CombatComp = Character->FindComponentByClass<UCombatComponent>();
			if (CombatComp)
			{
				CombatComp->EnableWeaponCollision();
			}
		}
	}
}

void UANS_WeaponCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		ANecPlayerCharacter* Character = Cast<ANecPlayerCharacter>(MeshComp->GetOwner());
		if (Character)
		{
			UCombatComponent* CombatComp = Character->FindComponentByClass<UCombatComponent>();
			if (CombatComp)
			{
				CombatComp->DisableWeaponCollision();
			}
		}
	}
}