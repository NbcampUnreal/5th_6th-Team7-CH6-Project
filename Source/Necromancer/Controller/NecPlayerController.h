#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NecPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UInGameHUDWidget;
class UReadyWidget;

UCLASS()
class NECROMANCER_API ANecPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANecPlayerController();

protected:
	virtual void BeginPlay() override;

	void CreateReadyWidgetForHost();

	void CreateInGameHUD();
	

	virtual void SetupInputComponent() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void OnRep_PlayerState() override;
	virtual void OnRep_Pawn() override;

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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> MenuAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> InteractAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Input")
	TObjectPtr<UInputAction> WheelAction;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UInGameHUDWidget> InGameHUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UInGameHUDWidget> InGameHUDWidgetInstance;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UReadyWidget> ReadyWidgetClass;

	UPROPERTY()
	TObjectPtr<UReadyWidget> ReadyWidgetInstance;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TArray<TSubclassOf<UUserWidget>> WidgetClasses;
	UPROPERTY()
	TArray<TObjectPtr<UReadyWidget>> WidgetInstances;
public:
	void OnStartGame();
};