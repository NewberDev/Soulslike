// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Monster/MonsterBase.h"
#include "Monster/MonsterCombatInterface.h"
#include "WildMan.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API AWildMan : public AMonsterBase
{
	GENERATED_BODY()

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
};
