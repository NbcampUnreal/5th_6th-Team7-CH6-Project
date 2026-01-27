#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NecPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
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
	void Lockon(const FInputActionValue& Value);

	UFUNCTION(Server, Reliable)
	void Server_SetMaxWalkSpeed(float NewSpeed);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* SpringArmComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier = 1.7f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;
};