// Curios_Item.cpp

#include "Item/Curios_Item.h"
#include "Components/StaticMeshComponent.h"

ACurios_Item::ACurios_Item()
{
	bStackable = true;
	MaxStackCount = 10; 

	CuriosMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CuriosMesh"));
	CuriosMesh->SetupAttachment(GetRootComponent());

	CuriosMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CuriosMesh->SetSimulatePhysics(true);
	CuriosMesh->SetGenerateOverlapEvents(false);

	CuriosMesh->SetCollisionResponseToAllChannels(ECR_Block);
	CuriosMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
}
