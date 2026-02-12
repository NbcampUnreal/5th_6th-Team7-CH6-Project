// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LobbyWidget.generated.h"

class UButton;
class UEditableText;
/**
 * 
 */
UCLASS()
class NECROMANCER_API ULobbyWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnCreateSessionButtonClicked();

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = USTitleWidget, Meta = (AllowPrivateAccess, BindWidget))
	TObjectPtr<UButton> CreateSessionButton;

};
