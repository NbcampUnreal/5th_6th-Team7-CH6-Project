//Armor_Item_Bass.cpp

#include "Item/Armor_Item_Bass.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"

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

	ACharacter* Character = Cast<ACharacter>(Equip_Owner);
	if (!Character) return;

	USkeletalMeshComponent* CharacterMesh = Character->GetMesh();
	if (!CharacterMesh) return;
	//CharacterMesh->SetVisibility(false, true);

	// 1️⃣ 부착
	AttachToComponent(CharacterMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale);

	// 2️⃣ 애니메이션 동기화 (핵심)
	ArmorMesh->SetLeaderPoseComponent(CharacterMesh);

	// 3️⃣ 필요하면 충돌 끄기
	ArmorMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AArmor_Item_Bass::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}