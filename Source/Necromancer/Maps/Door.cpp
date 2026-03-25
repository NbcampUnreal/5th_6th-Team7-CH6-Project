// Fill out your copyright notice in the Description page of Project Settings.

#include "Maps/Door.h"
#include "Components/TimelineComponent.h"
#include "Net/UnrealNetwork.h"
#include "AI/BossMonsterBase.h"
#include "AIController.h"
#include "Character/NecPlayerCharacter.h"

ADoor::ADoor()
{
	SetReplicates(true);
	AActor::SetReplicateMovement(true);
	DoorTimeline = CreateDefaultSubobject<UTimelineComponent>("DoorTimeline");
	NavLinkComp = CreateDefaultSubobject<UNavLinkCustomComponent>(TEXT("NavLink"));

	bDoorOpen = false;
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	UpdateFunctionFloat.BindDynamic(this, &ADoor::DoorOpenTimeLineFunc);
	if (DoorTimelineCurveFloat)
	{
		DoorTimeline->AddInterpFloat(DoorTimelineCurveFloat, UpdateFunctionFloat);
	}

	if (bDoorOpen)
	{
		float MinValue, MAxValue;
		DoorTimelineCurveFloat->GetValueRange(MinValue, MAxValue);
		DoorOpenTimeLineFunc(MinValue);
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
	}

	if (NavLinkComp)
	{
		NavLinkComp->SetLinkData(
			FVector(NavLinkOffset, 0.f, 0.f),
			FVector(-NavLinkOffset, 0.f, 0.f),
			ENavLinkDirection::BothWays
		);
		NavLinkComp->SetEnabled(true);
		NavLinkComp->SetMoveReachedLink(this, &ADoor::OnMoveReachedLink);
	}
}

void ADoor::Interact_Implementation(AActor* Interactor)
{
	if (!HasAuthority())
	{
		ANecPlayerCharacter* PlayerCharacter = Cast<ANecPlayerCharacter>(Interactor);
		if (!PlayerCharacter)
		{
			return;
		}
		PlayerCharacter->Server_TryInteract(this);
	}
	else {
		Server_ToggleDoor();
	}

}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADoor, bDoorOpen);
}

FText ADoor::GetInteractText_Implementation() const
{
	if (!bDoorOpen)
	{
		return FText::FromString(TEXT("열기"));
	}
	else
	{
		return FText::FromString(TEXT("닫기"));
	}
}

void ADoor::DoorOpenTimeLineFunc(float Output)
{
	const FRotator Value = FRotator(0, Output, 0);
	StaticMesh->SetRelativeRotation(Value);
}

void ADoor::Server_ToggleDoor_Implementation()
{
	bDoorOpen = !bDoorOpen;
	OnRep_DoorState();
	GetWorldTimerManager().ClearTimer(AICloseTimerHandle);
}

void ADoor::OnRep_DoorState()
{
	if (bDoorOpen)
	{
		DoorTimeline->Play();
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	else
	{
		DoorTimeline->Reverse();
		StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
}

void ADoor::OnMoveReachedLink(UNavLinkCustomComponent* LinkComp, UObject* PathComp, const FVector& DestPoint)
{
	if (!HasAuthority()) return;

	AAIController* AIC = Cast<AAIController>(PathComp ? PathComp->GetOuter() : nullptr);
	APawn* Pawn = AIC ? AIC->GetPawn() : nullptr;
	if (!Cast<ABossMonsterBase>(Pawn)) return;

	if (!bDoorOpen)
	{
		bDoorOpen = true;
		OnRep_DoorState();
	}

	GetWorldTimerManager().SetTimer(
		AICloseTimerHandle, this, &ADoor::CloseDoorForAI, AIOpenDuration, false);
}

void ADoor::CloseDoorForAI()
{
	if (HasAuthority() && bDoorOpen)
	{
		bDoorOpen = false;
		OnRep_DoorState();
	}
}
