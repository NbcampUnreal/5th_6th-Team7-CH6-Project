// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NecCompassWidget.generated.h"

UCLASS()
class NECROMANCER_API UNecCompassWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
    // 화살표 이미지 바인딩
    UPROPERTY(meta = (BindWidget))
    class UImage* CompassArrow;

    UPROPERTY()
    AActor* EndRoom;

    // EndRoom을 월드에서 찾기
    UFUNCTION(BlueprintCallable, Category = "Compass")
    void FindEndRoom();
};
