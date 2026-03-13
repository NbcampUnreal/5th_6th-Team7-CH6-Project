// Fill out your copyright notice in the Description page of Project Settings.

#include "Maps/Door.h"
#include "Components/TimelineComponent.h"
#include "Net/UnrealNetwork.h"

ADoor::ADoor()
{
	SetReplicates(true);
	AActor::SetReplicateMovement(true);
	DoorTimeline = CreateDefaultSubobject<UTimelineComponent>("DoorTimeline");

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
}

void ADoor::Interact_Implementation(AActor* Interactor)
{
	if (!HasAuthority())
	{
		Server_ToggleDoor();
		return;
	}

	Server_ToggleDoor();
}

void ADoor::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ADoor, bDoorOpen);
}

//FText ADoor::GetInteractText_Implementation() const
//{
//	if (!bDoorOpen)
//	{
//		return FText::FromString(TEXT("¿­±â"));
//	}
//	else
//	{
//		return FText::FromString(TEXT("´Ý±â"));
//	}
//}

void ADoor::DoorOpenTimeLineFunc(float Output)
{
	const FRotator Value = FRotator(0, Output, 0);
	StaticMesh->SetRelativeRotation(Value);
}

void ADoor::Server_ToggleDoor_Implementation()
{
	bDoorOpen = !bDoorOpen;
	OnRep_DoorState();
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
