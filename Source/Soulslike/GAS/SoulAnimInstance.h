// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "InputMappingContext.h"

#include "SoulAnimInstance.generated.h"


DECLARE_MULTICAST_DELEGATE(FOnOnWeaponeDelegate);
DECLARE_MULTICAST_DELEGATE(FOnOffWeaponDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackCheckDelegate);
/**
 * 
 */
UCLASS()
class SOULSLIKE_API USoulAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OnWeapon", Meta = (AllowPrivateAccess = true))
	UAnimMontage* OnWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OffWeapon", Meta = (AllowPrivateAccess = true))
	UAnimMontage* OffWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OffWeapon", Meta = (AllowPrivateAccess = true))
	UAnimMontage* ShieldBreakMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OffWeapon", Meta = (AllowPrivateAccess = true))
	UAnimMontage* StunMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OffWeapon", Meta = (AllowPrivateAccess = true))
	UAnimMontage* GuardMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn", Meta = (AllowPrivateAccess = true))
	bool IsPivotTurn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn", Meta = (AllowPrivateAccess = true))
	float ForwardInputValue;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turn", Meta = (AllowPrivateAccess = true))
	float RightInputValue;

	FVector SavedPivotForward;

public:
	//무기 상태 교체를 위한 델리게이트
	FOnOnWeaponeDelegate OnOnWeaponDelegate;
	FOnOffWeaponDelegate OnOffWeaponDelegate;
	FOnAttackCheckDelegate OnAttackCheckDelegate;
private:
	UFUNCTION()
	void AnimNotify_OnWeapon();

	UFUNCTION()
	void AnimNotify_OffWeapon();

	UFUNCTION()
	void AnimNotify_AttackCheck();

public:
	void PlayShieldBreak();

	void PlayStun();

	void PlayGuard();

	UFUNCTION(BlueprintCallable)
	void OnPivotTurn();

	UFUNCTION(BlueprintCallable)
	void OnStrafe();
	
};
