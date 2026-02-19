// Fill out your copyright notice in the Description page of Project Settings.


#include "WorldActor/InteractableActor.h"
#include "Character/NecPlayerCharacter.h"
#include "Components/BoxComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "NiagaraFunctionLibrary.h"

// Sets default values
AInteractableActor::AInteractableActor()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	CurrentHP = MaxHP;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	StaticMesh->SetupAttachment(Root);

	Collision = CreateDefaultSubobject<UBoxComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Root);
	Collision->OnComponentBeginOverlap.AddDynamic(
		this,
		&AInteractableActor::OnSphereOverlap);
	Collision->OnComponentEndOverlap.AddDynamic(
		this,
		&AInteractableActor::OnSphereEnd);

	InteractionCheckCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));
	InteractionCheckCollision->SetSphereRadius(CollisionRadius);
	InteractionCheckCollision->SetupAttachment(Root);
	InteractionCheckCollision->OnComponentBeginOverlap.AddDynamic(
		this,
		&AInteractableActor::OnCheckOverlap);
	InteractionCheckCollision->OnComponentEndOverlap.AddDynamic(
		this,
		&AInteractableActor::OnCheckEndOverlap);

	InteractWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidget"));
	InteractWidget->SetupAttachment(Root);
	InteractWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractWidget->SetDrawAtDesiredSize(true);
	InteractWidget->SetVisibility(false);

	InteractCheckWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractCheckWidget"));
	InteractCheckWidget->SetupAttachment(StaticMesh);
	InteractCheckWidget->SetWidgetSpace(EWidgetSpace::Screen);
	InteractCheckWidget->SetDrawAtDesiredSize(true);
	InteractCheckWidget->SetVisibility(false);

}

void AInteractableActor::BeginPlay()
{
	Super::BeginPlay();
}


void AInteractableActor::HandleDestroyed()
{
	bCanInteract = false;
	bCanTakeDamage = false;

	OnItemDestroyed();
}

void AInteractableActor::OnItemDestroyed_Implementation()
{
	if (HasAuthority())
	{
		Multicast_OnDeathEmitter();
		Destroy();
	}
}

void AInteractableActor::Multicast_OnDeathEmitter_Implementation()
{
	UParticleSystemComponent* Particle = nullptr;

	if (DestroyFX)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			DestroyFX,
			GetActorLocation(),
			FRotator::ZeroRotator,
			FVector(1.0f)
		);
	}
}

void AInteractableActor::OnSphereOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!bCanInteract) return;

	if (OtherActor->IsA<ANecPlayerCharacter>())
	{
		ANecPlayerCharacter* Player = Cast<ANecPlayerCharacter>(OtherActor);
		//부탁
		Player->SetInteractTarget(this);
	}
}

void AInteractableActor::OnSphereEnd(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (!bCanInteract) return;

	if (OtherActor->IsA<ANecPlayerCharacter>())
	{
		ANecPlayerCharacter* Player = Cast<ANecPlayerCharacter>(OtherActor);
		//부탁
		Player->ClearInteractTarget(this);
	}
}

void AInteractableActor::OnCheckOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (bCanTakeDamage)
	{
		return;
	}

	if (OtherActor->IsA<ANecPlayerCharacter>())
	{
		ShowInteractCheckWidget(true);
	}
}

void AInteractableActor::OnCheckEndOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	if (bCanTakeDamage)
	{
		return;
	}

	if (OtherActor->IsA<ANecPlayerCharacter>())
	{
		ShowInteractCheckWidget(false);
	}
}

void AInteractableActor::SetOutLineEnable(bool Enable)
{
	if (!bCanInteract) return;
	if (bCanTakeDamage) return;

	StaticMesh->SetRenderCustomDepth(Enable);
	ShowInteractWidget(Enable);
}

void AInteractableActor::SetPreviewWidgetEnable(bool Enable)
{
	if (bCanTakeDamage)
	{
		return;
	}

	ShowInteractCheckWidget(Enable);
}

float AInteractableActor::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent,
	class AController* EventInstigator, AActor* DamageCauser)
{
	if (!bCanTakeDamage)
		return 0.f;

	IInteractable::Execute_TakeDamage(this, DamageAmount, DamageCauser);

	return DamageAmount;
}


void AInteractableActor::Interact_Implementation(AActor* Interactor)
{
	if (!bCanInteract)return;

	//Todo 파생 클래스에서 구현
}

void AInteractableActor::TakeDamage_Implementation(float Damage, AActor* DamageCauser)
{
	if (!bCanTakeDamage)return;

	//Todo 파생 클래스에서 구현
}

void AInteractableActor::ShowInteractWidget(bool bVisible)
{
	if (InteractWidget)
	{
		InteractWidget->SetVisibility(bVisible);
	}
}

void AInteractableActor::ShowInteractCheckWidget(bool bVisible)
{
	if (InteractCheckWidget)
	{
		InteractCheckWidget->SetVisibility(bVisible);
	}
}
