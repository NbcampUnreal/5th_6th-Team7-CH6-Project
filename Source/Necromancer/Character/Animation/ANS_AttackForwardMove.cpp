#include "Character/Animation/ANS_AttackForwardMove.h"
#include "Character/NecPlayerCharacter.h"

void UANS_AttackForwardMove::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		ANecPlayerCharacter* Character = Cast<ANecPlayerCharacter>(MeshComp->GetOwner());
		if (Character && Character->IsLocallyControlled())
		{
			FVector ForwardVector = Character->GetActorForwardVector();
			Character->AddMovementInput(ForwardVector, MoveScale);
		}
	}
}