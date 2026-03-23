// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGame.generated.h"

class UTextBlock;
class UButton;

/**
 * 
 */
UCLASS()
class NECROMANCER_API UEndGame : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void OnRestartGameButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> RestartGameButton;


protected:
	UFUNCTION()
	void OnGoLobbyButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> GoLobbyButton;

public:
	void InitGameScore();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ClearLvDepthText;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TotalKillCountText;

};
