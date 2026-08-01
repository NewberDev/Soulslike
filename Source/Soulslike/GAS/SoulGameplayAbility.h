// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"

#include "SoulGameplayAbility.generated.h"





/**
 * 
 */
UCLASS()
class SOULSLIKE_API USoulGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

protected:

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Meta = (AllowPrivateAccess = true))
	bool OnPressed;

public:

	USoulGameplayAbility();

	
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;


	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;

	
};
