// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NecWaitingPlayerController.generated.h"


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

public:
	void StartGame();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UReadyWidget> HostReadyWidgetClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UReadyWidget> ClientReadyWidgetClass;

	UPROPERTY()
	TObjectPtr<UReadyWidget> ReadyWidgetInstance;
	
};
