// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(Client, Reliable)
	void ClientRPC_OnCharacterDead();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void ClientRPC_OnEnemyFind();

};
