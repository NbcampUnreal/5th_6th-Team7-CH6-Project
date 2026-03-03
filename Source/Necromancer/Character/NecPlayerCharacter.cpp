#include "Character/NecPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Controller/NecPlayerController.h"
#include "Game/NecPlayerState.h"
#include "EnhancedInputComponent.h"
#include "Necromancer.h"
#include "Component/StatComponent.h"
#include "Component/StaminaComponent.h"
#include "Component/PlayerMovementComponent.h"
#include "Component/CombatComponent.h"
#include "Component/TargetingComponent.h"
#include "Component/SoulComponent.h"
#include "Components/SphereComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMeshActor.h"
#include "Item/Weapon_Item_Base.h"
#include "Components/CapsuleComponent.h"


#include "WorldActor/Interactable.h"
#include "WorldActor/InteractableActor.h"

ANecPlayerCharacter::ANecPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;	

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);	

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->TargetArmLength = 300.0f;
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComponent->SetupAttachment(SpringArmComponent, USpringArmComponent::SocketName);
	CameraComponent->bUsePawnControlRotation = false;

	PlayerMovementComponent = CreateDefaultSubobject<UPlayerMovementComponent>(TEXT("PlayerMovementComponent"));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>(TEXT("TargetingComponent"));
	InventoryComponent = CreateDefaultSubobject<UNecInventoryComponent>(TEXT("NecInventoryComponent"));
	SoulComponent = CreateDefaultSubobject<USoulComponent>(TEXT("SoulComponent"));

	InteractionCheckCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));
	InteractionCheckCollision->SetSphereRadius(CollisionRadius);
	InteractionCheckCollision->SetupAttachment(GetCapsuleComponent());
	InteractionCheckCollision->OnComponentBeginOverlap.AddDynamic(
		this,
		&ANecPlayerCharacter::OnCheckOverlap);
	InteractionCheckCollision->OnComponentEndOverlap.AddDynamic(
		this,
		&ANecPlayerCharacter::OnCheckEndOverlap);


	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}

void ANecPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	InventoryComponent->LoadEquipment();
}

void ANecPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	
}

void ANecPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ANecPlayerController* PlayerController = Cast<ANecPlayerController>(GetController()))
		{
			if (PlayerController->MoveAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::Move
				);
			}

			if (PlayerController->LookAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::Look
				);
			}

			if (PlayerController->SprintAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::StartSprint
				);

				EnhancedInputComp->BindAction(
					PlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ANecPlayerCharacter::StopSprint
				);
			}

			if (PlayerController->AttackAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->AttackAction,
					ETriggerEvent::Started,
					this,
					&ANecPlayerCharacter::Attack
				);
			}

			if (PlayerController->GuardAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->GuardAction,
					ETriggerEvent::Started,
					this,
					&ANecPlayerCharacter::StartGuard
				);

				EnhancedInputComp->BindAction(
					PlayerController->GuardAction,
					ETriggerEvent::Completed,
					this,
					&ANecPlayerCharacter::StopGuard
				);
			}

			if (PlayerController->LockOnAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->LockOnAction,
					ETriggerEvent::Started,
					this,
					&ANecPlayerCharacter::LockOn
				);
			}

			if (PlayerController->MenuAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->MenuAction,
					ETriggerEvent::Started,
					this,
					&ANecPlayerCharacter::ToggleMenu
				);
			}

			if (PlayerController->InteractAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->InteractAction,
					ETriggerEvent::Started,
					this,
					&ANecPlayerCharacter::TryInteract
				);
			}
		}
	}	
}

void ANecPlayerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	LinkPlayerStateComponents();
}

void ANecPlayerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	LinkPlayerStateComponents();

}



void ANecPlayerCharacter::Move(const FInputActionValue& Value)
{
	if (IsValid(PlayerMovementComponent))
	{
		PlayerMovementComponent->ProcessMove(Value.Get<FVector2D>());
	}
}

void ANecPlayerCharacter::Look(const FInputActionValue& Value)
{
	if (IsValid(TargetingComponent) && TargetingComponent->GetCurrentTarget())
	{
		TargetingComponent->HandleLockOnInput(Value.Get<FVector2D>());
		return;
	}

	if (IsValid(PlayerMovementComponent))
	{
		PlayerMovementComponent->ProcessLook(Value.Get<FVector2D>());
	}
}

void ANecPlayerCharacter::StartSprint(const FInputActionValue& Value)
{
	if (!IsValid(StaminaComponent) || !IsValid(PlayerMovementComponent))
	{
		return;
	}

	if (StaminaComponent->IsExhausted())
	{
		if (PlayerMovementComponent->GetIsSprinting())
		{
			StopSprint(Value);
		}
		return;
	}

	PlayerMovementComponent->SetSprint(true);
	GetCharacterMovement()->MaxWalkSpeed = PlayerMovementComponent->GetSprintSpeed();

	StaminaComponent->StartStaminaDrain(10.0f);

	Server_SetSprint(true);
}

void ANecPlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	if (IsValid(PlayerMovementComponent))
	{		
		PlayerMovementComponent->SetSprint(false);
		GetCharacterMovement()->MaxWalkSpeed = PlayerMovementComponent->GetNormalSpeed();

		bool bResetExhaustion = StaminaComponent->GetCurrentStamina() > 0.0f;
		StaminaComponent->StopStaminaDrain(bResetExhaustion);

		Server_SetSprint(false);
	}
}

void ANecPlayerCharacter::Attack(const FInputActionValue& Value)
{
	if (IsValid(CombatComponent))
	{
		CombatComponent->Attack();
	}
}

void ANecPlayerCharacter::StartGuard(const FInputActionValue& Value)
{
	if (CombatComponent)
	{		
		CombatComponent->SetGuard(true);
	}
}

void ANecPlayerCharacter::StopGuard(const FInputActionValue& Value)
{
	if (CombatComponent)
	{
		CombatComponent->SetGuard(false);
	}
}

void ANecPlayerCharacter::LockOn(const FInputActionValue& Value)
{
	if (IsValid(CombatComponent))
	{
		TargetingComponent->ToggleLockOn();
	}
}

void ANecPlayerCharacter::ToggleMenu(const FInputActionValue& Value)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	if (!PC || !InGameMenuClass)
	{
		return;
	}

	if (!InGameMenuInstance)
	{
		InGameMenuInstance = CreateWidget<UUserWidget>(PC, InGameMenuClass);
	}

	if (InGameMenuInstance->IsInViewport())
	{
		InGameMenuInstance->RemoveFromParent();

		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}
	else
	{
		InGameMenuInstance->AddToViewport();

		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(InGameMenuInstance->TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);

		PC->SetInputMode(InputMode);
		PC->SetShowMouseCursor(true);
	}
}

void ANecPlayerCharacter::TryInteract()
{
	if (InteractTarget) {
		if (InteractTarget->Implements<UInteractable>())
		{
			IInteractable::Execute_Interact(InteractTarget, this);
		}
	}
}

void ANecPlayerCharacter::Server_Interact_Implementation(AActor* Target)
{
	Cast<AInteractableActor>(Target)->Interact_Internal(this);
}

void ANecPlayerCharacter::HandleDeath()
{
	if (HasAuthority())
	{		
		//AController* CurrentController = GetController();

		//if (CurrentController)
		//{
		//	//CurrentController->UnPossess();			
		//}

		Multicast_HandleDeath();
		//멀티 수정 필요
		SoulComponent->EnterDownState();

		/*GetWorldTimerManager().SetTimer(
			DeathTimerHandle,
			this,
			&ANecPlayerCharacter::EndGame,
			5.0f,
			false
		);*/
	}
}

void ANecPlayerCharacter::Multicast_HandleDeath_Implementation()
{
	if (GetCapsuleComponent())
	{
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (GetMesh())
	{
		GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
		GetMesh()->SetSimulatePhysics(true);
	}
}

void ANecPlayerCharacter::HandleRevive()
{
	if (HasAuthority())
	{
		Multicast_HandleRevive();
	}
}

void ANecPlayerCharacter::Multicast_HandleRevive_Implementation()
{
	USkeletalMeshComponent* MeshComp = GetMesh();
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();

	if (!MeshComp || !CapsuleComp)
		return;

	// 1️⃣ pelvis 기준 캡슐 이동
	FVector PelvisLocation = MeshComp->GetSocketLocation(TEXT("pelvis"));
	PelvisLocation.Z += CapsuleComp->GetScaledCapsuleHalfHeight();

	CapsuleComp->SetWorldLocation(PelvisLocation);

	FRotator NewRot(0.f, MeshComp->GetComponentRotation().Yaw, 0.f);
	CapsuleComp->SetWorldRotation(NewRot);

	MeshComp->SetSimulatePhysics(false);
	MeshComp->bBlendPhysics = false;

	MeshComp->AttachToComponent(CapsuleComp, FAttachmentTransformRules::SnapToTargetIncludingScale);
	MeshComp->SetRelativeLocation(FVector(0.f, 0.f, -CapsuleComp->GetScaledCapsuleHalfHeight()));
	MeshComp->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	MeshComp->SetCollisionProfileName(TEXT("CharacterMesh"));
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	StatComponent->SetCurrentHealth(1.f);
}



void ANecPlayerCharacter::EndGame()
{
	if (HasAuthority())
	{
		UKismetSystemLibrary::QuitGame(this, nullptr, EQuitPreference::Quit, false);
	}
}

void ANecPlayerCharacter::LinkPlayerStateComponents()
{
	ANecPlayerState* PS = GetPlayerState<ANecPlayerState>();
	if (PS)
	{
		StatComponent = PS->GetStatComponent();
		StaminaComponent = PS->GetStaminaComponent();

		if (StatComponent && StaminaComponent)
		{
			if (HasAuthority())
			{ 
				StatComponent->BindToOwnerPawn(this);

				StatComponent->OnDeath.RemoveDynamic(this, &ANecPlayerCharacter::HandleDeath);
				StatComponent->OnDeath.AddDynamic(this, &ANecPlayerCharacter::HandleDeath);

				UE_LOG(LogTemp, Warning, TEXT("[Server] Successfully linked Component: %s"), *GetName());
			}
		}
		InventoryComponent = PS->GetInventoryComponent();
		if (InventoryComponent) {
			InventoryComponent->OnEquipmentUpdated.AddDynamic(
				CombatComponent,
				&UCombatComponent::SetCurrentWeapon
			);
			if (HasAuthority())
			{
			}
		}
		if (SoulComponent) {
			SoulComponent->OnReviveRequested.AddDynamic(
				this,
				&ANecPlayerCharacter::HandleRevive
			);
		}
	}
}

void ANecPlayerCharacter::Server_EquipWeapon_Implementation(AWeapon_Item_Base* WeaponToEquip)
{
	if (CombatComponent && WeaponToEquip)
	{
		CombatComponent->EquipWeapon(WeaponToEquip);
	}
}

FGenericTeamId ANecPlayerCharacter::GetGenericTeamId() const
{
	return FGenericTeamId(TEAM_ID_PLAYER);
}

void ANecPlayerCharacter::SetLockOn(bool bEnable)
{
	if (bEnable)
	{
		GetCharacterMovement()->bOrientRotationToMovement = false;
		bUseControllerRotationYaw = true;
	}
	else
	{
		GetCharacterMovement()->bOrientRotationToMovement = true;
		bUseControllerRotationPitch = false;
		bUseControllerRotationYaw = false;
		bUseControllerRotationRoll = false;
	}

}

AActor* ANecPlayerCharacter::GetCurrentEquipmentActor(EEquipmentSlot Slot)
{
	return InventoryComponent->GetEquipmentActor(Slot);
}

void ANecPlayerCharacter::Server_SetSprint_Implementation(bool bIsSprinting)
{
	if (!IsValid(PlayerMovementComponent) || !IsValid(StaminaComponent))
	{
		return;
	}

	if (bIsSprinting)
	{
		if (!StaminaComponent->IsExhausted() && StaminaComponent->GetCurrentStamina() > 0.0f)
		{
			GetCharacterMovement()->MaxWalkSpeed = PlayerMovementComponent->GetSprintSpeed();
			StaminaComponent->StartStaminaDrain(10.0f);
		}
		else
		{
			GetCharacterMovement()->MaxWalkSpeed = PlayerMovementComponent->GetNormalSpeed();
			StaminaComponent->StopStaminaDrain(false);
		}
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = PlayerMovementComponent->GetNormalSpeed();
		bool bResetExhaustion = StaminaComponent->GetCurrentStamina() > 0.0f;
		StaminaComponent->StopStaminaDrain(bResetExhaustion);
	}
}

void OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

void OnSphereEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
}
