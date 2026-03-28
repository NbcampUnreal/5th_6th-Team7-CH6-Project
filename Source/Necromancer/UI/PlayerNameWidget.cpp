// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/PlayerNameWidget.h"

#include "Components/TextBlock.h"

void UPlayerNameWidget::SetPlayerName(FString Name)
{
    if (PlayerNameText)
    {
        FString CombinedString = FString::Printf(TEXT("%s"), *Name);
        PlayerNameText->SetText(FText::FromString(CombinedString));
    }
}

void UPlayerNameWidget::SetPlayerNameColor(bool bIsDead)
{
    if (PlayerNameText)
    {
        PlayerNameText->SetColorAndOpacity(bIsDead ? FSlateColor(FColor(255, 0, 0, 255)) : FSlateColor(FColor(255, 255, 255, 255)));
    }
}
