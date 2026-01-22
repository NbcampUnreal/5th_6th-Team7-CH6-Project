#pragma once

#include "CoreMinimal.h"
#include "GAS/Base/BaseCharacter.h"
#include "NecPlayerCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputAction;
struct FInputActionValue;

UCLASS()
class NECROMANCER_API ANecPlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:
	ANecPlayerCharacter();
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

protected:	
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* JumpAction;
		
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* LookAction;
		
	UPROPERTY(EditAnywhere, Category = "Input")
	UInputAction* MouseLookAction;

protected:	
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:	
	void Move(const FInputActionValue& Value);		
	void Look(const FInputActionValue& Value);

public:	
	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoMove(float Right, float Forward);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoLook(float Yaw, float Pitch);

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpStart();

	UFUNCTION(BlueprintCallable, Category = "Input")
	virtual void DoJumpEnd();

public:	
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }	
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};