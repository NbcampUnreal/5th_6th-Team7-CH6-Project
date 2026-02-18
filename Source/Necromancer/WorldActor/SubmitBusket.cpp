// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActor/SubmitBusket.h"
#include "Character/NecPlayerCharacter.h"
#include "GridInventory/BucketInventoryComponent.h"
#include "GridInventory/NecInventoryComponent.h"
#include "UI/SubmitWidgetHub.h"
#include "Net/UnrealNetwork.h"


ASubmitBusket::ASubmitBusket()
{
    PrimaryActorTick.bCanEverTick = false;

    BucketInventory = CreateDefaultSubobject<UBucketInventoryComponent>(TEXT("BucketInventory"));
    BucketInventory->SetRequircost(100);
    BucketInventory->OnSubmit.AddDynamic(
        this,
        &ASubmitBusket::Submit
    );
    bReplicates = true;
    bIsActive = true;
}

void ASubmitBusket::BeginPlay()
{
    Super::BeginPlay();
}

void ASubmitBusket::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ASubmitBusket, bIsActive);
}

void ASubmitBusket::Interact_Implementation(AActor* Interactor)
{
    UE_LOG(LogTemp, Warning, TEXT("SubmitBusket Interacted"));
    if (!bIsActive)
    {
        return; // 비활성 상태면 아무것도 안함
    }

    if (!Interactor)
    {
        return;
    }

    ANecPlayerCharacter* PlayerCharacter = Cast<ANecPlayerCharacter>(Interactor);
    if (!PlayerCharacter)
    {
        return;
    }

    // 플레이어 인벤토리 가져오기
    UNecInventoryComponent* Inventory = PlayerCharacter->GetInventoryComponent();
    if (!Inventory)
    {
        return;
    }
    Inventory->ToggleSubmitUI(BucketInventory);
}

void ASubmitBusket::Submit() {
    if (!HasAuthority())
    {
        Server_Submit();
    }
    else {
        Submit_Internal();
    }
}

void ASubmitBusket::Server_Submit_Implementation()
{
    Submit_Internal();
}

void ASubmitBusket::Submit_Internal()
{
    bIsActive = false;
}

void ASubmitBusket::OnRep_IsActive()
{
    // 클라이언트에서 Active 상태 변경 시 처리
    // 예: 머티리얼 변경, 이펙트 ON/OFF, UI 표시 등

    if (bIsActive)
    {
        UE_LOG(LogTemp, Warning, TEXT("SubmitBusket Activated"));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("SubmitBusket Deactivated"));
    }
}

void ASubmitBusket::OnSphereEnd(UPrimitiveComponent* OverlappedComp,
    AActor* OtherActor,
    UPrimitiveComponent* OtherComp,
    int32 OtherBodyIndex)
{
    Super::OnSphereEnd(OverlappedComp,
        OtherActor, OtherComp, OtherBodyIndex);

    if (!bIsActive)
    {
        return; // 비활성 상태면 아무것도 안함
    }

    if (!OtherActor)
    {
        return;
    }

    ANecPlayerCharacter* PlayerCharacter = Cast<ANecPlayerCharacter>(OtherActor);
    if (!PlayerCharacter)
    {
        return;
    }

    // 플레이어 인벤토리 가져오기
    UNecInventoryComponent* Inventory = PlayerCharacter->GetInventoryComponent();
    if (!Inventory)
    {
        return;
    }
    if (Inventory->GetSubmitWidget()) {
        Inventory->ToggleSubmitUI(BucketInventory);
    }    
}