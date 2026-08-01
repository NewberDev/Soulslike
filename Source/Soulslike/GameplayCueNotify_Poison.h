// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GameplayCueNotify_Poison.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API AGameplayCueNotify_Poison : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
public:

	AGameplayCueNotify_Poison();

	virtual bool OnActive_Implementation(AActor* MyTarget, const FGameplayCueParameters& Parameters);
};
