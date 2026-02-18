#include "Character/Animation/AN_CheckComboInput.h"
#include "Character/NecPlayerCharacter.h"
#include "Component/CombatComponent.h"

void UAN_CheckComboInput::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp && MeshComp->GetOwner())
	{
		ANecPlayerCharacter* Character = Cast<ANecPlayerCharacter>(MeshComp->GetOwner());
		if (Character)
		{
			UCombatComponent* CombatComp = Character->FindComponentByClass<UCombatComponent>();
			if (CombatComp)
			{
				CombatComp->CheckComboTransition();
			}
		}
	}
}