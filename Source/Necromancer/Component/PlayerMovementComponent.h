#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class NECROMANCER_API UPlayerMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:		
	UPlayerMovementComponent();

protected:
	virtual void BeginPlay() override;

public:
	void ProcessMove(const FVector2D& Value);
	void ProcessLook(const FVector2D& Value);
	void SetSprint(bool bShouldSprint);

	float GetNormalSpeed() const { return NormalSpeed; }
	float GetSprintSpeed() const { return SprintSpeed; }

protected:
	UPROPERTY()
	TObjectPtr<ACharacter> OwnerCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float NormalSpeed = 400.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeedMultiplier = 1.7f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	float SprintSpeed;
};