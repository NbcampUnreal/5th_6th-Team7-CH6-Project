//Armor_Item_Bass.cpp

#include "Item/Armor_Item_Bass.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "Character/NecPlayerCharacter.h"
#include "Component/StatComponent.h"

AArmor_Item_Bass::AArmor_Item_Bass()
{
	bReplicates = true;

	ArmorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmorMesh"));
	SetRootComponent(ArmorMesh);

	Defense = 0.2f;
}

void AArmor_Item_Bass::Equip(AActor* Equip_Owner)
{
	Super::Equip(Equip_Owner);

	//ACharacter* Character = Cast<ACharacter>(Equip_Owner);
	//if (!Character) return;

	//USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	//if (!CharacterMesh) return;
	////CharacterMesh->SetVisibility(false, true);

	ANecPlayerCharacter* PlayerCharacter = Cast<ANecPlayerCharacter>(Equip_Owner);
	if (!PlayerCharacter)
	{
		return;
	}

	USkeletalMeshComponent* MainMesh = PlayerCharacter->GetMesh();
	if (!MainMesh)
	{
		return;
	}

	switch (ArmorSlotType)
	{
	case EEquipmentSlot::Head:
		if (PlayerCharacter->HeadMesh)
		{
			PlayerCharacter->HeadMesh->SetVisibility(false);
			if (PlayerCharacter->GetStatComponent())
			{
				PlayerCharacter->GetStatComponent()->AddArmor(HeadArmor);
			}
		}
		break;
	case EEquipmentSlot::Body:
		if (PlayerCharacter->BodyMesh)
		{
			PlayerCharacter->BodyMesh->SetVisibility(false);
			if (PlayerCharacter->GetStatComponent())
			{
				PlayerCharacter->GetStatComponent()->AddArmor(BodyArmor);
			}
		}
		break;
	case EEquipmentSlot::Legs:
		if (PlayerCharacter->LegMesh)
		{
			PlayerCharacter->LegMesh->SetVisibility(false);
			if (PlayerCharacter->GetStatComponent())
			{
				PlayerCharacter->GetStatComponent()->AddArmor(LegArmor);
			}
		}
		break;
	default:
		break;
	}

	// 1️⃣ 부착
	AttachToComponent(MainMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// 2️⃣ 애니메이션 동기화 (핵심)
	ArmorMesh->SetLeaderPoseComponent(MainMesh);

	// 3️⃣ 필요하면 충돌 끄기
	ArmorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AArmor_Item_Bass::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}