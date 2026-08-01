// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "MyAnimInstance.generated.h"

//11/26 - 콤보어택 : 델리게이트
DECLARE_MULTICAST_DELEGATE(FOnNextAttackCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnAttackHitCheckDelegate);
DECLARE_MULTICAST_DELEGATE(FOnBulletFireDelegate);
DECLARE_MULTICAST_DELEGATE(FOnOnWeaponeDelegate);
DECLARE_MULTICAST_DELEGATE(FOnOffWeaponDelegate);


/**
 * 
 */
UCLASS()
class SOULSLIKE_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	UAnimMontage* AttackMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roll", Meta = (AllowPrivateAccess = true))
	UAnimMontage* RollMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Roll", Meta = (AllowPrivateAccess = true))
	UAnimMontage* FireMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OnWeapon", Meta = (AllowPrivateAccess = true))
	UAnimMontage* OnWeaponMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "OffWeapon", Meta = (AllowPrivateAccess = true))
	UAnimMontage* OffWeaponMontage;

public:
	/*
	UPROPERTY(BlueprintReadWrite) //블루 프린트 접근 키워드
	bool isJumpStart = false;
	*/

	//11/26 - 콤보 어택
	FOnNextAttackCheckDelegate OnNextAttackCheckDelegate;
	FOnAttackHitCheckDelegate OnAttackHitCheckDelegate;
	FOnBulletFireDelegate OnBulletFireDelegate;

	//무기 들기 집어넣기
	FOnOnWeaponeDelegate OnOnWeaponDelegate;
	FOnOffWeaponDelegate OnOffWeaponDelegate;

	void PlayAttackMontage();

	void PlayRollMontage();

	void PlayFireMontage();

	void PlayOnWeaponMontage();

	void PlayOffWeaponMontage();
	
	// 11/26 -콤보 어택-  함수들 ->
	UFUNCTION()
	void JumpToAttackMontageSection(int32 NewSection);
private:
	UFUNCTION() //언리얼 연결 함수라는 의미인듯?
	void AnimNotify_AttackHitCheck(); //우리가 만든 노티파이의 이름으로 호출하기
	//애니메이션 몽타주에 AttackHitCheck이라는 이름의 노티파이의 신호가 올때 호출되는 함수

	UFUNCTION()
	void AnimNotify_NextAttackCheck();

	UFUNCTION()
	void AnimNotify_BulletFire();

	UFUNCTION()
	void AnimNotify_OnWeapon();

	UFUNCTION()
	void AnimNotify_OffWeapon();

	FName GetAttackMontageSectionName(int32 Section);
	//색션이름을 전달하는 함수로 정의하여

	// <- 11/26

	bool IsAttackMontagePlaying();

public:
	bool IsRollMontagePlaying();

};
