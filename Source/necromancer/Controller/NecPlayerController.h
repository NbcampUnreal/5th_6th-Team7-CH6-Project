#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "NecPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;

UCLASS()
class NECROMANCER_API ANecPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:	

	virtual void BeginPlay() override;

	virtual void SetupInputComponent() override;	

protected:
	UPROPERTY(EditAnywhere, Category = "Input|Input Mappings")
	UInputMappingContext* DefaultMappingContexts;
};