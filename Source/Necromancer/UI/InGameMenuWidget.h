#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InGameMenuWidget.generated.h"

class UButton;

UCLASS()
class NECROMANCER_API UInGameMenuWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnExitButtonClicked();

protected:	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ExitButton;


protected:
	UFUNCTION()
	void OnInviteFriendButtonClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> InviteFriendButton;
};
