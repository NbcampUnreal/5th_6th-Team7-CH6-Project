#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NecPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UInGameHUDWidget;

UCLASS()
class NECROMANCER_API ANecPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	virtual void OnRep_PlayerState() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> SprintAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> AttackAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> GuardAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> LockOnAction;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInGameHUDWidget> InGameHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UInGameHUDWidget> InGameHUDWidgetInstance;
};