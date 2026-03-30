// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NecWaitingPlayerController.generated.h"



class UInputMappingContext;
class UInputAction;

class UReadyWidget;
/**
 * 
 */
UCLASS()
class NECROMANCER_API ANecWaitingPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ANecWaitingPlayerController();

protected:
	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;


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
	TObjectPtr<UInputAction> MenuAction;

public:
	void StartGame();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UReadyWidget> HostReadyWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UReadyWidget> ClientReadyWidgetClass;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UReadyWidget> ReadyWidgetInstance;

	
	UPROPERTY(EditDefaultsOnly, Category = "BGM")
	TObjectPtr<USoundBase> WaitingBGM;
};
