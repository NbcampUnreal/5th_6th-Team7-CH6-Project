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

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SpectatingTargetUpAction;

	UFUNCTION()
	void SpectatingTargetUp();

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> SpectatingTargetDownAction;

	UFUNCTION()
	void SpectatingTargetDown();

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


protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
public:
	/// <summary>
	/// 빙의 해제 및 카메라 이동 시퀀스 수행
	/// </summary>
	UFUNCTION()
	void OnPlayerDeath();
	UFUNCTION(Server, Reliable)
	void Server_NotifyDeath();

	/// <summary>
	/// 사망시, 관전자를 탐색하여 타겟을 이동 수행
	/// </summary>
	UFUNCTION(Client, Reliable)
	void Client_HandleDeath(AActor* TargetToSpectate);

	UFUNCTION(Server, Reliable)
	void Server_RequestSpectatingTarget(AActor* InSpectatingTarget, bool bIsUp);

	/// <summary>
	/// Client_HandleDeath() || UpdateSpectateRotation() 에서 호출
	/// </summary>
	/// <param name="TargetToSpectate"></param>
	void SetSpectateTargetInternal(AActor* TargetToSpectate);

	void UpdateSpectateRotation();

protected:
	UPROPERTY(Replicated)
	bool bIsSpectating = false;

	FTimerHandle SpectateRotationTimerHandle;
	AActor* SpectatingTarget;
	UPROPERTY()
	class ANecPlayerState* CurSpectatingTargetState;

#pragma region Personal Achievement
public:
	UFUNCTION(Client, Reliable)
	void Client_NotifyMonsterKill();

#pragma endregion
};