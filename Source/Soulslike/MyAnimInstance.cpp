// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAnimInstance.h"

void UMyAnimInstance::PlayAttackMontage()
{
	if (!AttackMontage) //애니메이션 몽타주가 잘 선택되어 있는가 확인
	{
		//UE_LOG(LogTemp, Warning, TEXT(""));
		return;
	}

	if (Montage_IsPlaying(AttackMontage)) //사용하려는 몽타주가 이미 실행중이라면 
	{
		return; //이미 동작중임 다시 실행하지마! 의 뜻
	}

	Montage_Play(AttackMontage); //위에서 문제가 없었다면 실행하시오!
} 
//이제 이 애니메이션 몽타주를 받아온 구성을 캐릭터의 C++에 전달해야한다.

void UMyAnimInstance::PlayRollMontage()
{
	UE_LOG(LogTemp, Warning, TEXT("UMyAnimInstance::PlayRollMontage"));
	if (!RollMontage) //애니메이션 몽타주가 잘 선택되어 있는가 확인
	{
		UE_LOG(LogTemp, Warning, TEXT("UMyAnimInstance::PlayRollMontage_Invalid"));
		return;
	}

	if (Montage_IsPlaying(RollMontage)) //사용하려는 몽타주가 이미 실행중이라면 
	{
		return; //이미 동작중임 다시 실행하지마! 의 뜻
	}

	Montage_Play(RollMontage, 1.3f); //위에서 문제가 없었다면 실행하시오!
}
//이제 이 애니메이션 몽타주를 받아온 구성을 캐릭터의 C++에 전달해야한다.

void UMyAnimInstance::PlayFireMontage()
{
	UE_LOG(LogTemp, Warning, TEXT("UMyAnimInstance::PlayFireMontage"));
	if (!FireMontage) //애니메이션 몽타주가 잘 선택되어 있는가 확인
	{
		UE_LOG(LogTemp, Warning, TEXT("UMyAnimInstance::PlayFireMontage_Invalid"));
		return;
	}

	if (Montage_IsPlaying(FireMontage)) //사용하려는 몽타주가 이미 실행중이라면 
	{
		return; //이미 동작중임 다시 실행하지마! 의 뜻
	}

	Montage_Play(FireMontage);
}

void UMyAnimInstance::PlayOnWeaponMontage()
{
	UE_LOG(LogTemp, Warning, TEXT("UMyAnimInstance::PlayOnWeaponMontage"));
	if (!OnWeaponMontage) //애니메이션 몽타주가 잘 선택되어 있는가 확인
	{
		UE_LOG(LogTemp, Warning, TEXT("UMyAnimInstance::PlayOnWeaponMontage_Invalid"));
		return;
	}

	if (Montage_IsPlaying(OnWeaponMontage)) //사용하려는 몽타주가 이미 실행중이라면 
	{
		return; //이미 동작중임 다시 실행하지마! 의 뜻
	}

	Montage_Play(OnWeaponMontage);
}

void UMyAnimInstance::PlayOffWeaponMontage()
{
	UE_LOG(LogTemp, Warning, TEXT("UMyAnimInstance::PlayOffWeaponMontage"));
	if (!OffWeaponMontage) //애니메이션 몽타주가 잘 선택되어 있는가 확인
	{
		UE_LOG(LogTemp, Warning, TEXT("UMyAnimInstance::PlayOffWeaponMontage_Invalid"));
		return;
	}

	if (Montage_IsPlaying(OffWeaponMontage)) //사용하려는 몽타주가 이미 실행중이라면 
	{
		return; //이미 동작중임 다시 실행하지마! 의 뜻
	}

	Montage_Play(OffWeaponMontage);
}

//11/26 - 콤보 어택 -
void UMyAnimInstance::JumpToAttackMontageSection(int32 NewSection)
{
	Montage_JumpToSection(GetAttackMontageSectionName(NewSection));
	//위 함수로 인해 노티파이의 안에서 버튼을 누르면 다음 섹션(동작)으로 이동하는 것을 말함
}

void UMyAnimInstance::AnimNotify_AttackHitCheck()
{
	UE_LOG(LogTemp, Log, TEXT("UMyAnimInstance::AnimNotify_AttackHitCheck"));
	OnAttackHitCheckDelegate.Broadcast(); //델리게이트에 등록된 함수들이 실행된다.
}

void UMyAnimInstance::AnimNotify_NextAttackCheck()
{
	UE_LOG(LogTemp, Log, TEXT("UMyAnimInstance::AnimNotify_NextAttackCheck"));
	OnNextAttackCheckDelegate.Broadcast(); 
}

void UMyAnimInstance::AnimNotify_BulletFire()
{
	OnBulletFireDelegate.Broadcast();
}

FName UMyAnimInstance::GetAttackMontageSectionName(int32 Section)
{
	return FName(*FString::Printf(TEXT("Attack%d"), Section));
}

bool UMyAnimInstance::IsAttackMontagePlaying()
{
	return Montage_IsPlaying(AttackMontage);
}

bool UMyAnimInstance::IsRollMontagePlaying()
{
	return Montage_IsPlaying(RollMontage);
}

void UMyAnimInstance::AnimNotify_OnWeapon()
{
	OnOnWeaponDelegate.Broadcast();
}

void UMyAnimInstance::AnimNotify_OffWeapon()
{
	OnOffWeaponDelegate.Broadcast();
}
