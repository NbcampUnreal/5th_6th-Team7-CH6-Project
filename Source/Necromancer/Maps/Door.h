// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WorldActor/InteractableActor.h"
#include "Components/TimelineComponent.h"
#include "NavLinkCustomComponent.h"
#include "Door.generated.h"

UCLASS()
class NECROMANCER_API ADoor : public AInteractableActor
{
	GENERATED_BODY()

public:
	ADoor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Interact_Implementation(AActor* Interactor) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	// virtual FText GetInteractText_Implementation() const override;

protected:
	UFUNCTION()
	virtual void DoorOpenTimeLineFunc(float Output);

	//������ ����
	UPROPERTY(ReplicatedUsing = OnRep_DoorState, EditAnywhere, BlueprintReadWrite)
	bool bDoorOpen = false;

private:
	//������ ���� ����
	FOnTimelineFloat UpdateFunctionFloat;

	//������ ����� CurveFloat
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	UCurveFloat* DoorTimelineCurveFloat;

	//������ ����� Timeline
	UPROPERTY(EditDefaultsOnly, Category = "Door Action", meta = (AllowPrivateAccess = true))
	UTimelineComponent* DoorTimeline;

	UFUNCTION(Server, Reliable)
	void Server_ToggleDoor();

	UFUNCTION()
	void OnRep_DoorState();

	// NavLink (보스 문 통과)
	UPROPERTY(VisibleAnywhere, Category = "Navigation", meta = (AllowPrivateAccess = true))
	TObjectPtr<UNavLinkCustomComponent> NavLinkComp;

	UPROPERTY(EditDefaultsOnly, Category = "Navigation", meta = (AllowPrivateAccess = true))
	float NavLinkOffset = 250.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Navigation", meta = (AllowPrivateAccess = true))
	float AIOpenDuration = 4.0f;

	FTimerHandle AICloseTimerHandle;

	void OnMoveReachedLink(UNavLinkCustomComponent* LinkComp, UObject* PathComp, const FVector& DestPoint);
	void CloseDoorForAI();
};
