//Steel_Armor.cpp

#include "Item/Steel_Armor.h"

#include "Components/SkeletalMeshComponent.h"
#include "UObject/ConstructorHelpers.h"

ASteel_Armor::ASteel_Armor()
{
	Defense = 0.35f;

	//static ConstructorHelpers::FObjectFinder<USkeletalMesh> ArmorMeshAsset(TEXT(""));

	//if (ArmorMeshAsset.Succeeded())
	//{
	//	ArmorMesh->SetSkeletalMesh(ArmorMeshAsset.Object);
	//}

	//static ConstructorHelpers::FObjectFinder<USoundBase> ArmorSoundAsset(TEXT(""));

	//if (ArmorSoundAsset.Succeeded())
	//{
	//	ArmorSound = ArmorSoundAsset.Object;
	//}
}
