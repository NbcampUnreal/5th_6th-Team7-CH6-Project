// Sword_Item.cpp

#include "Item/Sword_Item.h"

#include "GameFramework/Pawn.h"
#include "Kismet/GameplayStatics.h"

ASword_Item::ASword_Item()
{
	Damage = 20.f;

	////매쉬
	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> SwordMeshAsset(	TEXT(""));

	//if (SwordMeshAsset.Succeeded() && WeaponMesh)
	//{
	//	WeaponMesh->SetSkeletalMesh(SwordMeshAsset.Object);
	//}

	////사운드
	//static ConstructorHelpers::FObjectFinder<USoundBase> SwordAttackSound(TEXT(""));

	//if (SwordAttackSound.Succeeded())
	//{
	//	AttackSound = SwordAttackSound.Object;
	//}
}

