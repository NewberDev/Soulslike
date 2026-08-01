// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AIAnimInstance.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);

UCLASS()
class SOULSLIKE_API UAIAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roll", Meta = (AllowPrivateAccess = true))
	UAnimMontage* RollMontage;

public:

	FOnAttackHitCheckDelegate OnAttackHitCheckDelegate;

	UFUNCTION()
	void AnimNotify_AttackHitCheck();

	bool IsRollMontagePlaying();
	
};
