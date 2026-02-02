#include "UI/InGameMenuWidget.h"
#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

void UInGameMenuWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ExitButton)
	{
		ExitButton->OnClicked.RemoveDynamic(this, &UInGameMenuWidget::OnExitButtonClicked);
		ExitButton->OnClicked.AddDynamic(this, &UInGameMenuWidget::OnExitButtonClicked);
	}
}

void UInGameMenuWidget::OnExitButtonClicked()
{
	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}