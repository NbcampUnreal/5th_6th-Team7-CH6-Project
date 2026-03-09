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
class  USoulComponent;
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

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HandleRevive();

	void EndGame();

	void LinkPlayerStateComponents();

	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(AWeapon_Item_Base* WeaponToEquip);

public:
	UFUNCTION(Server, Reliable)
	void Server_SetSprint(bool bIsSprinting);
	
	//팀 이름 반환
	virtual FGenericTeamId GetGenericTeamId() const override;

	UStatComponent* GetStatComponent() const { return StatComponent; }
	UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }
	UNecInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }

	void SetLockOn(bool bEnable);

	

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

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> InGameMenuClass;
		
	UPROPERTY()
	TObjectPtr<UUserWidget> InGameMenuInstance;

	UPROPERTY(EditAnywhere, Category = "Test")
	float InteractRange = 200.0f;

	

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
	void CycleTarget(bool bNext);
#pragma endregion

#pragma region SoulComponent
protected:
	TObjectPtr<USoulComponent> SoulComponent;
public:
	USoulComponent* GetSoulComponent() const;

	void AddSubmissionReward();
#pragma endregion

};