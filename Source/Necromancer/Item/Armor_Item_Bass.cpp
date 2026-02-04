//Armor_Item_Bass.cpp

#include "Item/Armor_Item_Bass.h"

#include "Components/SkeletalMeshComponent.h"
#include "Net/UnrealNetwork.h"

AArmor_Item_Bass::AArmor_Item_Bass()
{
	bReplicates = true;

	ArmorMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ArmorMesh"));
	SetRootComponent(ArmorMesh);

	Defense = 0.2f;
}

void AArmor_Item_Bass::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

}