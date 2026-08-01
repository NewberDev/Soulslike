// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBossAnimInstance.h"

void UMyBossAnimInstance::AnimNotify_SpawnMagic()
{
	UE_LOG(LogTemp, Log, TEXT("UMyBossAnimInstance::AnimNotify_MagicSpawn"));
	OnMagicSpawnDelegate.Broadcast();

}

void UMyBossAnimInstance::AnimNotify_AttackHitCheck1()
{
	OnAttackHit1Delegate.Broadcast();
}

void UMyBossAnimInstance::AnimNotify_AttackHitCheck2()
{
	OnAttackHit2Delegate.Broadcast();
}


void UMyBossAnimInstance::AnimNotify_AOEHitCheck()
{
	OnAOEHitDelegate.Broadcast();
}

void UMyBossAnimInstance::AnimNotify_GrabCheck()
{
	OnGrabDelegate.Broadcast();
}

bool UMyBossAnimInstance::IsGuardMontagePlaying()
{
	return Montage_IsPlaying(GuardMontage);
}

bool UMyBossAnimInstance::IsMagicMontagePlaying()
{
	return Montage_IsPlaying(MagicMontage);
}

void UMyBossAnimInstance::SetOnIk(bool On_Off)
{
	OnIK = On_Off;
}

