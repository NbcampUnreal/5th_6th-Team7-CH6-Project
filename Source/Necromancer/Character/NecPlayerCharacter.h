#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NecPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UStatComponent;
class UStaminaComponent;
struct FInputActionValue;

UCLASS()
class NECROMANCER_API ANecPlayerCharacter : public ACharacter
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

	UFUNCTION(Server, Reliable)
	void Server_SetMaxWalkSpeed(float NewSpeed);

	void LinkPlayerStateComponents();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Camera")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Camera")
	TObjectPtr<UCameraComponent> CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Stat")
	TObjectPtr<UStatComponent> StatComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Component|Stat")
	TObjectPtr<UStaminaComponent> StaminaComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier = 1.7f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;
};