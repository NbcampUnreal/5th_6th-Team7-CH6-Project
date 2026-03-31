// Fill out your copyright notice in the Description page of Project Settings.

#include "UI/NecCompassWidget.h"
#include "Components/Image.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

void UNecCompassWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    // EndRoom이 없으면 매 틱마다 찾기 시도
    if (!EndRoom)
    {
        FindEndRoom();
        return;
    }

    APlayerController* PC = GetOwningPlayer();
    if (!PC) return;

    APawn* PlayerPawn = PC->GetPawn();
    if (!PlayerPawn) return;

    FVector PlayerLocation = PlayerPawn->GetActorLocation();
    FVector EndRoomLocation = EndRoom->GetActorLocation();

    // 플레이어 → EndRoom 방향 벡터 (Z 무시)
    FVector Direction = (EndRoomLocation - PlayerLocation);
    Direction.Z = 0.f;
    Direction.Normalize();

    // 플레이어 카메라의 Yaw 기준으로 상대 각도 계산
    FRotator ControlRotation = PC->GetControlRotation();
    float ControlYaw = ControlRotation.Yaw;

    float TargetYaw = FMath::RadiansToDegrees(FMath::Atan2(Direction.Y, Direction.X));

    // 화면 기준 상대 각도 (북쪽 기준 보정 -90도)
    float RelativeAngle = TargetYaw - ControlYaw;

    // 화살표 이미지 회전 적용
    if (CompassArrow)
    {
        CompassArrow->SetRenderTransformAngle(RelativeAngle);
    }
}

void UNecCompassWidget::FindEndRoom()
{
    // 블루프린트 클래스 이름으로 월드에서 EndRoom 탐색
    TArray<AActor*> FoundActors;
    UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("EndRoom"), FoundActors);

    if (FoundActors.Num() > 0)
    {
        EndRoom = FoundActors[0];
    }
}