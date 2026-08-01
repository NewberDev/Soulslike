// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyBossAnimInstance.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnMagicSpawnDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHit1Delegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHit2Delegate);
DECLARE_MULTICAST_DELEGATE(FOnAOEHitDelegate);
DECLARE_MULTICAST_DELEGATE(FOnGrabDelegate);

/**
 * 
 */
UCLASS()
class SOULSLIKE_API UMyBossAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* MagicMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Montage", Meta = (AllowPrivateAccess = true))
	UAnimMontage* GuardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CR_IK", Meta = (AllowPrivateAccess = true))
	bool OnIK = false;

public:
	FOnMagicSpawnDelegate OnMagicSpawnDelegate;
	FOnAttackHit1Delegate OnAttackHit1Delegate;
	FOnAttackHit2Delegate OnAttackHit2Delegate;
	FOnAOEHitDelegate OnAOEHitDelegate;
	FOnGrabDelegate OnGrabDelegate;

	UFUNCTION()
	void AnimNotify_SpawnMagic();

	UFUNCTION()
	void AnimNotify_AttackHitCheck1();

	UFUNCTION()
	void AnimNotify_AttackHitCheck2();

	UFUNCTION()
	void AnimNotify_AOEHitCheck();

	UFUNCTION()
	void AnimNotify_GrabCheck();

	bool IsGuardMontagePlaying();

	bool IsMagicMontagePlaying();

	UFUNCTION(BlueprintCallable)
	void SetOnIk(bool On_Off);
	
};
