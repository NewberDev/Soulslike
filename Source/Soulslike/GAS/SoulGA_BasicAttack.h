// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GAS/SoulGameplayAbility.h"
#include "SoulGA_BasicAttack.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API USoulGA_BasicAttack : public USoulGameplayAbility
{
	GENERATED_BODY()


	
protected:

	UFUNCTION(BlueprintCallable)
	void ComboAttackCheck();

	
};
