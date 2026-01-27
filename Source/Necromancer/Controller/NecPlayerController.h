#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NecPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UCLASS()
class NECROMANCER_API ANecPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ANecPlayerController();

protected:
	virtual void SetupInputComponent() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* GuardAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	UInputAction* LockonAction;
};