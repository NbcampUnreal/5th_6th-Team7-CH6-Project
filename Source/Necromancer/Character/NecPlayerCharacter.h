#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NecPlayerCharacter.generated.h"

UCLASS()
class NECROMANCER_API ANecPlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:	
	ANecPlayerCharacter();

protected:	
	virtual void BeginPlay() override;	
	virtual void Tick(float DeltaTime) override;		
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


};