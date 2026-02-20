#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "GridInventory/NecInventoryComponent.h"
#include "NecPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStatComponent;
class UStaminaComponent;
class UPlayerMovementComponent;
class UCombatComponent;
class UUserWidget;
class UTargetingComponent;
class AWeapon_Item_Base;
struct FInputActionValue;

UCLASS()
class NECROMANCER_API ANecPlayerCharacter : public ACharacter , public IGenericTeamAgentInterface
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
	void Interact();
	

	UFUNCTION()
	void HandleDeath();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_HandleDeath();

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

	UFUNCTION(Server, Reliable)
	void Server_Interact(AActor* Target);

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

private:
	AActor* InteractTarget = nullptr;
public:
	void SetInteractTarget(AActor* Target) {
		InteractTarget = Target;
	}
	void ClearInteractTarget(AActor* Target) {
		if (InteractTarget == Target)
		{
			InteractTarget = nullptr;
		}
	}
};