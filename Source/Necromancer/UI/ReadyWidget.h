// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ReadyWidget.generated.h"


class UTextBlock;
class UButton;

/**
 *
 */
UCLASS()
class NECROMANCER_API UReadyWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
	void OnStartGameButtonClicked();

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> StartGameButton;


protected:
	UFUNCTION()
	void OnInviteFriendButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> InviteFriendButton;

protected:
	UFUNCTION()
	void UpdatePlayerControllerCount(int32 PlayerCount);

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerCountText;
};
