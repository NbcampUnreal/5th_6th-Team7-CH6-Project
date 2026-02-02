// Fill out your copyright notice in the Description page of Project Settings.


#include "GridInventory/ItemInstance/TestInventoryItem.h"
#include "ItemInstanceComponent.h"
#include "ItemInstance.h"

ATestInventoryItem::ATestInventoryItem()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor")); // 테스트용
	Mesh->SetSimulatePhysics(true);

	// 아이템 인스턴스 컴포넌트
	ItemInstanceComponent = CreateDefaultSubobject<UItemInstanceComponent>(TEXT("ItemInstanceComponent"));
}

// Sets default values
ATestInventoryItem::ATestInventoryItem(UItemInstance* NewItemInstance)
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	// Mesh
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Root);
	Mesh->SetCollisionProfileName(TEXT("PhysicsActor")); // 테스트용
	Mesh->SetSimulatePhysics(true);

	// 아이템 인스턴스 컴포넌트
	ItemInstanceComponent = CreateDefaultSubobject<UItemInstanceComponent>(TEXT("ItemInstanceComponent"));
	if (NewItemInstance) {
		ItemInstanceComponent->Initialize(NewItemInstance);
	}
}

// Called when the game starts or when spawned
void ATestInventoryItem::BeginPlay()
{
	Super::BeginPlay();
	if (!ItemInstanceComponent->GetItemInstance()) {
		UItemInstance* NewItemInstance = NewObject<UItemInstance>(this);
		ItemInstanceComponent->Initialize(NewItemInstance);
		NewItemInstance->InitializeIdentity("0_000");
	}
}

// Called every frame
void ATestInventoryItem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

