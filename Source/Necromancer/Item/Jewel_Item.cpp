// Jewel_Item.cpp

#include "Item/Jewel_Item.h"
#include "Components/StaticMeshComponent.h"

AJewel_Item::AJewel_Item()
{
	bStackable = true;
	MaxStackCount = 99;

	SellPrice = 10;
	bCanBuyFromShop = true;
	bCanSellToShop = true;

	JewelMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("JewelMesh"));
	JewelMesh->SetupAttachment(GetRootComponent());

	JewelMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	JewelMesh->SetSimulatePhysics(true);
	JewelMesh->SetGenerateOverlapEvents(false);

	JewelMesh->SetCollisionResponseToAllChannels(ECR_Block);
	JewelMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	JewelType = EJewelType::Ruby;
}
