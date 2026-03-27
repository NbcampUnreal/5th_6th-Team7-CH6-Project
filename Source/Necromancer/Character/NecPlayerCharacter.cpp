#include "Character/NecPlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Controller/NecPlayerController.h"
#include "Controller/NecWaitingPlayerController.h"

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
#include "Net/UnrealNetwork.h"

#include "WorldActor/Interactable.h"
#include "WorldActor/InteractableActor.h"
#include "NiagaraFunctionLibrary.h"

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
	//SoulComponent = CreateDefaultSubobject<USoulComponent>(TEXT("SoulComponent"));

	InteractionCheckCollision = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionCollision"));
	InteractionCheckCollision->SetSphereRadius(CollisionRadius);
	InteractionCheckCollision->SetupAttachment(GetCapsuleComponent());
	InteractionCheckCollision->OnComponentBeginOverlap.AddDynamic(
		this,
		&ANecPlayerCharacter::OnSphereOverlap);
	InteractionCheckCollision->OnComponentEndOverlap.AddDynamic(
		this,
		&ANecPlayerCharacter::OnSphereEnd);

	HeadMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HeadMesh"));
	HeadMesh->SetupAttachment(GetMesh());

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetMesh());

	LegMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegMesh"));
	LegMesh->SetupAttachment(GetMesh());

	GetMesh()->bHiddenInGame = true;
	GetMesh()->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
}


void ANecPlayerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, RemoteViewRot);
}

void ANecPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	InventoryComponent->LoadEquipment();

	if (GetMesh())
	{
		HeadMesh->SetLeaderPoseComponent(GetMesh());
		BodyMesh->SetLeaderPoseComponent(GetMesh());
		LegMesh->SetLeaderPoseComponent(GetMesh());
	}
}

void ANecPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), GetEnumText(GetLocalRole()), this, FColor::White, DeltaTime);

	ReplicateRemoteViewRot();
}

FString ANecPlayerCharacter::GetEnumText(ENetRole _Role)
{
	switch (_Role)
	{
	case ROLE_None:
		return "None";
	case ROLE_SimulatedProxy:
		return "SimulatedProxy";
	case ROLE_AutonomousProxy:
		return "AutonomousProxy";
	case ROLE_Authority:
		return "Authority";
	default:
		return "ERROR";
	}
}

void ANecPlayerCharacter::ReplicateRemoteViewRot()
{
	if (HasAuthority())
	{
		RemoteViewRot = GetControlRotation();
	}
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

			if (PlayerController->WheelAction)
			{
				EnhancedInputComp->BindAction(
					PlayerController->WheelAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::Action_CycleTarget
				);
			}
		} 
		else if (ANecWaitingPlayerController* WaitingPlayerController = Cast<ANecWaitingPlayerController>(GetController()))
		{
			if (WaitingPlayerController->MoveAction)
			{
				EnhancedInputComp->BindAction(
					WaitingPlayerController->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::Move
				);
			}

			if (WaitingPlayerController->LookAction)
			{
				EnhancedInputComp->BindAction(
					WaitingPlayerController->LookAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::Look
				);
			}

			if (WaitingPlayerController->SprintAction)
			{
				EnhancedInputComp->BindAction(
					WaitingPlayerController->SprintAction,
					ETriggerEvent::Triggered,
					this,
					&ANecPlayerCharacter::StartSprint
				);

				EnhancedInputComp->BindAction(
					WaitingPlayerController->SprintAction,
					ETriggerEvent::Completed,
					this,
					&ANecPlayerCharacter::StopSprint
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
	if (IsValid(CombatComponent) && CombatComponent->IsPlayingAttackMontage())
	{
		return;
	}

	if (bIsHit)
	{
		return;
	}

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

	if (IsValid(CombatComponent) && CombatComponent->IsAttacking())
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
	if (StatComponent->GetStatus()==ECharacterStatus::Down) {
		if (HasAuthority())
		{
			SoulComponent->TryRevive();
		}
		else
		{
			Server_RequestRevive();
		}
		return;
	}

	AActor* Target = CurrentTarget.Get();

	if (!Target) return;
	if (!Target->Implements<UInteractable>()) return;

	IInteractable::Execute_Interact(Target, this);

	//if (HasAuthority())
	//{
	//	IInteractable::Execute_Interact(Target, this);
	//}
	//else
	//{
	//	//IInteractable::Execute_Interact(Target, this);
	//	Server_TryInteract(Target);

	//}
	CleanupInvalidTargets();

}

void ANecPlayerCharacter::Server_TryInteract_Implementation(AActor* Target)
{
	if (!Target) return;
	if (!Target->Implements<UInteractable>()) return;

	IInteractable::Execute_Interact(Target, this);
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

		CurrentTarget = nullptr;
		InteractTargets.Empty();
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

		if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp->StopMovementImmediately();
			MoveComp->DisableMovement();
			MoveComp->GravityScale = 0.0f;
		}
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
		StatComponent->SetCurrentHealth(50.f);
		StatComponent->SetStatus(ECharacterStatus::Alive);
	}
}

void ANecPlayerCharacter::Server_RequestRevive_Implementation()
{
	SoulComponent->TryRevive();
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

	// 부활시에 무브먼트 컴포넌트 원복
	if (UCharacterMovementComponent* MoveComp = GetCharacterMovement())
	{
		MoveComp->SetMovementMode(EMovementMode::MOVE_Walking);
		MoveComp->GravityScale = 1.0f;
	}
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

			StatComponent->OnDamageReceived.RemoveDynamic(this, &ANecPlayerCharacter::PlayBloodEffect);
			StatComponent->OnDamageReceived.AddDynamic(this, &ANecPlayerCharacter::PlayBloodEffect);
			StatComponent->OnDamageReceived.RemoveDynamic(this, &ANecPlayerCharacter::OnDamageReceived);
			StatComponent->OnDamageReceived.AddDynamic(this, &ANecPlayerCharacter::OnDamageReceived);
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
		SoulComponent = PS->GetSoulComponent();
		if (SoulComponent) {
			SoulComponent->OnReviveRequested.AddDynamic(
				this,
				&ANecPlayerCharacter::HandleRevive
			);
		}
	}
}

void ANecPlayerCharacter::PlayBloodEffect(float DamageAmount, FVector HitLocation, bool bPoiseBroken)
{
	if (DamageAmount <= 0.0f || !BloodEffectFX)
	{
		return;
	}

	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), BloodEffectFX, HitLocation);
}

void ANecPlayerCharacter::OnDamageReceived(float DamageAmount, FVector HitLocation, bool bPoiseBroken)
{
	if (DamageAmount <= 0.0f)
	{
		return;
	}

	if (!bPoiseBroken)
	{
		return;
	}

	if (HitMontage || GuardHitMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			if (IsValid(CombatComponent))
			{
				CombatComponent->ResetCombatState();
				CombatComponent->DisableWeaponCollision();
			}

			bIsHit = true;

			if (!AnimInstance->OnMontageEnded.IsAlreadyBound(this, &ANecPlayerCharacter::OnHitMontageEnded))
			{
				AnimInstance->OnMontageEnded.AddDynamic(this, &ANecPlayerCharacter::OnHitMontageEnded);
			}
			
			if (CombatComponent->IsGuarding())
			{
				PlayAnimMontage(GuardHitMontage);
			}
			else
			{
				PlayAnimMontage(HitMontage);
			}
		}
	}
}

void ANecPlayerCharacter::OnHitMontageEnded(UAnimMontage* Montage, bool bInterupped)
{
	if (Montage == HitMontage || Montage == GuardHitMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			if (!AnimInstance->Montage_IsPlaying(HitMontage) || !AnimInstance->Montage_IsPlaying(GuardHitMontage))
			{
				bIsHit = false;

				if (IsValid(CombatComponent) && CombatComponent->IsGuarding())
				{
					CombatComponent->UpdateGuardVisuals();
				}
			}
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

bool ANecPlayerCharacter::GetISDead()
{
	return false;
}

AActor* ANecPlayerCharacter::GetCurrentEquipmentActor(EEquipmentSlot Slot)
{
	return InventoryComponent->GetEquipmentActor(Slot);
}

void ANecPlayerCharacter::AddInteractTarget(AActor* Target) {
	if (!StatComponent) return;
	if (StatComponent->GetStatus()!=ECharacterStatus::Alive)
		return;

	if (!IsValid(Target)) return;

	InteractTargets.AddUnique(Target);

	if (!CurrentTarget.IsValid())
	{
		CurrentTarget = Target;
	}
	CleanupInvalidTargets();
}

void ANecPlayerCharacter::RemoveInteractTarget(AActor* Target) {
	if (!IsValid(Target)) return;

	InteractTargets.Remove(Target);

	if (CurrentTarget.Get() == Target)
	{
		SelectFallbackTarget();
	}
	CleanupInvalidTargets();
}

void ANecPlayerCharacter::SelectFallbackTarget()
{
	CurrentTarget = nullptr;

	// 유효한 것 중 첫 번째 선택
	for (const TWeakObjectPtr<AActor>& Target : InteractTargets)
	{
		if (Target.IsValid())
		{
			CurrentTarget = Target;
			return;
		}
	}
}

void ANecPlayerCharacter::CleanupInvalidTargets()
{
	InteractTargets.RemoveAll(
		[](const TWeakObjectPtr<AActor>& Target)
		{
			return !Target.IsValid();
		}
	);

	if (!CurrentTarget.IsValid())
	{
		SelectFallbackTarget();
	}
}

AActor* ANecPlayerCharacter::GetCurrentInteractTarget() const
{
	return CurrentTarget.IsValid() ? CurrentTarget.Get() : nullptr;
}

void ANecPlayerCharacter::CycleTarget(bool bNext)
{
	CleanupInvalidTargets();

	if (InteractTargets.Num() == 0)
		return;

	int32 CurrentIndex = InteractTargets.IndexOfByKey(CurrentTarget);

	if (CurrentIndex == INDEX_NONE)
	{
		CurrentTarget = InteractTargets[0];
		return;
	}

	int32 NewIndex;

	if (bNext)
		NewIndex = (CurrentIndex + 1) % InteractTargets.Num();
	else
		NewIndex = (CurrentIndex - 1 + InteractTargets.Num()) % InteractTargets.Num();

	CurrentTarget = InteractTargets[NewIndex];
}

inline void ANecPlayerCharacter::Action_CycleTarget(const FInputActionValue& Value)
{
	float Axis = Value.Get<float>();

	if (Axis > 0.f)
	{
		CycleTarget(true);
	}
	else if (Axis < 0.f)
	{
		CycleTarget(false);
	}
}

void ANecPlayerCharacter::Server_SetSprint_Implementation(bool bIsSprinting)
{
	if (!IsValid(PlayerMovementComponent) || !IsValid(StaminaComponent))
	{
		return;
	}

	if (bIsSprinting && IsValid(CombatComponent) && CombatComponent->IsAttacking())
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

void  ANecPlayerCharacter::OnSphereOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!StatComponent)
		return;
	if (StatComponent->GetStatus() == ECharacterStatus::Alive)
		return;
	if (!OtherActor) return;
	if (ANecPlayerCharacter* Player = Cast<ANecPlayerCharacter>(OtherActor))
	{
		Player->AddInteractTarget(this);
	}
}

void  ANecPlayerCharacter::OnSphereEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (!StatComponent)
		return;
	if (StatComponent->GetStatus() == ECharacterStatus::Alive)
		return;
	if (!OtherActor) return;
	if (ANecPlayerCharacter* Player = Cast<ANecPlayerCharacter>(OtherActor))
	{
		Player->RemoveInteractTarget(this);
	}
}

void ANecPlayerCharacter::OnCheckOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
}

void ANecPlayerCharacter::OnCheckEndOverlap(UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
}

void ANecPlayerCharacter::Interact_Implementation(AActor* Interactor)
{
	if (StatComponent->GetStatus() == ECharacterStatus::Alive)
		return;

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
		ANecPlayerCharacter* PlayerCharacter = Cast<ANecPlayerCharacter>(Interactor);
		if (!PlayerCharacter)
		{
			return;
		}
		FSoulBattery Battery;

		if (PlayerCharacter->GetSoulComponent()->TakeReserveBattery(Battery))
		{
			SoulComponent->AddReserveBattery(Battery);
		}
		else
		{
			// 여분 배터리가 없음
		}
	}
}

FText ANecPlayerCharacter::GetInteractText_Implementation() const
{
	return FText::FromString(TEXT("부활"));
}


USoulComponent* ANecPlayerCharacter::GetSoulComponent() const
{
	return SoulComponent;
}

void ANecPlayerCharacter::AddSubmissionReward()
{
	if (StatComponent->GetStatus() == ECharacterStatus::Death)
		return;
	if (SoulComponent) {
		SoulComponent->AddReserveBattery(FSoulBattery());
	}
}