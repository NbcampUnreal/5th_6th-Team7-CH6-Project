#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "GridInventory/NecInventoryComponent.h"
#include "WorldActor/Interactable.h"
#include "NecPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStatComponent;
class UStaminaComponent;
class UPlayerMovementComponent;
class UCombatComponent;
class UUserWidget;
class UTargetingComponent;
class USphereComponent;
class AWeapon_Item_Base;
class USoulComponent;
class UNiagaraSystem;
class UWidgetComponent;
struct FInputActionValue;


UCLASS()
class NECROMANCER_API ANecPlayerCharacter : public ACharacter , public IGenericTeamAgentInterface, public IInteractable
{
	GENERATED_BODY()

public:	
	ANecPlayerCharacter();

protected:	
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	void GetPlayerSteamName();

	UFUNCTION()
	void Move(const FInputActionValue& Value);

	UFUNCTION()
	void Look(const FInputActionValue& Value);

	UFUNCTION()
	void StartSprint(const FInputActionValue& Value);

	UFUNCTION()
	void StopSprint(const FInputActionValue& Value);

	UFUNCTION()
	void Attack(const FInputActionValue& Value);

	UFUNCTION()
	void StartGuard(const FInputActionValue& Value);

	UFUNCTION()
	void StopGuard(const FInputActionValue& Value);

	UFUNCTION()
	void LockOn(const FInputActionValue& Value);

	UFUNCTION()
	void ToggleMenu(const FInputActionValue& Value);

	UFUNCTION()
	void Action_CycleTarget(const FInputActionValue& Value);

	

	UFUNCTION()
	void HandleDeath();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HandleDeath();

	UFUNCTION()
	void HandleRevive();

	UFUNCTION(Server, Reliable)
	void Server_RequestRevive();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HandleRevive();

	void EndGame();

	void LinkPlayerStateComponents();

	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(AWeapon_Item_Base* WeaponToEquip);

	UFUNCTION()
	void PlayBloodEffect(float DamageAmount, FVector HitLocation, bool bPoiseBroken);

	UFUNCTION()
	void OnDamageReceived(float DamageAmount, FVector HitLocation, bool bPoiseBroken);

	UFUNCTION()
	void OnHitMontageEnded(UAnimMontage* Montage, bool bInterupped);

public:
	UFUNCTION(Server, Reliable)
	void Server_SetSprint(bool bIsSprinting);
	
	//팀 이름 반환
	virtual FGenericTeamId GetGenericTeamId() const override;

	UStatComponent* GetStatComponent() const { return StatComponent; }
	UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	UPlayerMovementComponent* GetPlayerMovementComponent() const { return PlayerMovementComponent; }
	UNecInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	void SetLockOn(bool bEnable);

	bool IsHit() const { return bIsHit; }

	bool GetISDead();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Stat")
	TObjectPtr<UStatComponent> StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Stat")
	TObjectPtr<UStaminaComponent> StaminaComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Movement")
	TObjectPtr<UPlayerMovementComponent> PlayerMovementComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Combat")
	TObjectPtr<UCombatComponent> CombatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Combat")
	TObjectPtr<UTargetingComponent> TargetingComponent;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UWidgetComponent> PlayerNameWidgetComponent;


	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InGameMenuClass;
		
	UPROPERTY()
	TObjectPtr<UUserWidget> InGameMenuInstance;

	UPROPERTY(EditAnywhere, Category = "Test")
	float InteractRange = 200.0f;

	UPROPERTY(EditDefaultsOnly, Category = "VFX")
	TObjectPtr<UNiagaraSystem> BloodEffectFX;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> HitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	TObjectPtr<UAnimMontage> GuardHitMontage;

	UPROPERTY(VisibleAnywhere, Category = "Animation")
	bool bIsHit = false;

	FTimerHandle DeathTimerHandle;

protected:
	//인벤토리
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Inventory")
	TObjectPtr<UNecInventoryComponent> InventoryComponent;
public:
	AActor* GetCurrentEquipmentActor(EEquipmentSlot Slot);
	
#pragma region interact
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TObjectPtr<USphereComponent> InteractionCheckCollision;

	float CollisionRadius = 700.0f;

	UFUNCTION(BlueprintCallable)
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	virtual void OnSphereEnd(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	UFUNCTION(BlueprintCallable)
	virtual void OnCheckOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	UFUNCTION(BlueprintCallable)
	virtual void OnCheckEndOverlap(UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);

	

public:	
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual FText GetInteractText_Implementation() const override;
protected:
	UFUNCTION()
	void TryInteract();
private:
	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentTarget;
	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> InteractTargets;

public:
	UFUNCTION(Server, Reliable)
	void Server_TryInteract(AActor* Target);
	void AddInteractTarget(AActor* Target);
	void RemoveInteractTarget(AActor* Target);
	void SelectFallbackTarget();
	void CleanupInvalidTargets();
	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentInteractTarget() const;
	UFUNCTION(BlueprintCallable)
	bool HasMultipleInteractTargets() const {
		if (InteractTargets.Num() > 1)
			return true;
		return false;
	}
	void CycleTarget(bool bNext);
#pragma endregion

#pragma region SoulComponent
protected:
	TObjectPtr<USoulComponent> SoulComponent;
public:
	USoulComponent* GetSoulComponent() const;

	void AddSubmissionReward();
#pragma endregion

protected:
	FString GetEnumText(ENetRole Role);

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


	/// <summary>
	/// 플레이어 닉네임 위젯의 회전
	/// </summary>
	void UpdatePlayerNameCompRot();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_SetPlayerNameColor(bool bIsDead);
public:
	/// <summary>
	/// 컨트롤러 회전값 복제용 -> 클라이언트의 관전에 필요(not server)
	/// </summary>
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Networking")
	FRotator RemoteViewRot;

protected:
		void ReplicateRemoteViewRot();

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Mesh")
	USkeletalMeshComponent* HeadMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Mesh")
	USkeletalMeshComponent* BodyMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Mesh")
	USkeletalMeshComponent* LegMesh;
};