#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "NecPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStatComponent;
class UStaminaComponent;
class UPlayerMovementComponent;
class UCombatComponent;
struct FInputActionValue;

UCLASS()
class NECROMANCER_API ANecPlayerCharacter : public ACharacter , public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:	
	ANecPlayerCharacter();

protected:	
	virtual void BeginPlay() override;
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
	void Guard(const FInputActionValue& Value);

	UFUNCTION()
	void LockOn(const FInputActionValue& Value);

	void LinkPlayerStateComponents();

public:
	UFUNCTION(Server, Reliable)
	void Server_SetSprint(bool bIsSprinting);
	
	//팀 이름 반환
	virtual FGenericTeamId GetGenericTeamId() const override;

	UStatComponent* GetStatComponent() const { return StatComponent; }
	UStaminaComponent* GetStaminaComponent() const { return StaminaComponent; }

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
};