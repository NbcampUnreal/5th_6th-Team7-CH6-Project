#include "UI/InGameMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "GameInstance/NecAFGameInstance.h"

void UInGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ExitButton)
	{
		ExitButton->OnClicked.RemoveDynamic(this, &UInGameMenuWidget::OnExitButtonClicked);
		ExitButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnExitButtonClicked);
	}

	if (InviteFriendButton)
	{
		InviteFriendButton->OnClicked.RemoveDynamic(this, &UInGameMenuWidget::OnInviteFriendButtonClicked);
		InviteFriendButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnInviteFriendButtonClicked);
	}
}

void UInGameMenuWidget::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UInGameMenuWidget::OnInviteFriendButtonClicked()
{
	UNecAFGameInstance* NecAFGameInstance = Cast<UNecAFGameInstance>(GetGameInstance());
	if (NecAFGameInstance)
	{
		NecAFGameInstance->InviteFriend();
	}
}
