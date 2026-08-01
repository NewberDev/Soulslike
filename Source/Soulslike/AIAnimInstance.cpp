// Fill out your copyright notice in the Description page of Project Settings.


#include "AIAnimInstance.h"


void UAIAnimInstance::AnimNotify_AttackHitCheck()
{
	//UE_LOG(LogTemp, Log, TEXT("UMyAnimInstance::AnimNotify_AttackHitCheck"));
	OnAttackHitCheckDelegate.Broadcast(); //델리게이트에 등록된 함수들이 실행된다.
}

bool UAIAnimInstance::IsRollMontagePlaying()
{
	return Montage_IsPlaying(RollMontage);
}
