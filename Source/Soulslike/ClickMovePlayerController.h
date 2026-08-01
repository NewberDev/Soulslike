// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputMappingContext.h" //입력을 위한 추가
#include "ClickMovePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API AClickMovePlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	UInputAction* Mouse2PressedAction;

	UPROPERTY(EditAnywhere)
	UInputMappingContext* InputMappingContext;
	
public:
	AClickMovePlayerController();


protected:
	bool isMouse2Pressed;

	void OnMouse2Pressed();
	void OnMouse2Released();
	void SetNewDestination(const FVector Destination);
	void MoveToMouseCursor();


	virtual void SetupInputComponent() override;

	virtual void PlayerTick(float DeltaTime) override;

public:
	void OnPossess(APawn* InPawn) override;

};
