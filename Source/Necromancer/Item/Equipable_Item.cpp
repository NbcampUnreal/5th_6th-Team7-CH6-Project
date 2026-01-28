// Equipable_Item.cpp

#include "Item/Equipable_Item.h"

#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

AEquipable_Item::AEquipable_Item()
{
	bReplicates = true;
	SetReplicateMovement(true);
}

void AEquipable_Item::OnEquip(ACharacter* OwnerCharacter)
{
	if (!OwnerCharacter)
	{
		return;
	}

	// 서버 기준으로만 상태 변경
	if (!HasAuthority())
	{
		return;
	}

	bEquipped = true;
	EquippedCharacter = OwnerCharacter;

	AttachToCharacter(OwnerCharacter);
}

void AEquipable_Item::OnUnequip()
{
	if (!HasAuthority())
	{
		return;
	}

	bEquipped = false;
	EquippedCharacter = nullptr;

	DetachFromCharacter();
}

void AEquipable_Item::AttachToCharacter(ACharacter* OwnerCharacter)
{
	if (!OwnerCharacter)
	{
		return;
	}

	SetOwner(OwnerCharacter);
	SetInstigator(OwnerCharacter);

	FAttachmentTransformRules AttachRules(
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::SnapToTarget,
		EAttachmentRule::KeepWorld,
		true
	);

	AttachToComponent(OwnerCharacter->GetMesh(),AttachRules,AttachSocketName);
}

void AEquipable_Item::DetachFromCharacter()
{
	SetOwner(nullptr);
	SetInstigator(nullptr);

	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld,true);

	DetachFromActor(DetachRules);
}

void AEquipable_Item::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AEquipable_Item, bEquipped);
}
