// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulAnimInstance.h"
#include "SoulCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"


//노티파이시 실행 함수
void USoulAnimInstance::AnimNotify_OnWeapon()
{
	OnOnWeaponDelegate.Broadcast();
}

void USoulAnimInstance::AnimNotify_OffWeapon()
{
	OnOffWeaponDelegate.Broadcast();
}

void USoulAnimInstance::AnimNotify_AttackCheck()
{
	UE_LOG(LogTemp, Log, TEXT("Notify Called"));
	OnAttackCheckDelegate.Broadcast();
}


//몽타주 재생 함수
void USoulAnimInstance::PlayShieldBreak()
{
	if (ShieldBreakMontage)
	{
		Montage_Play(ShieldBreakMontage, 0.8f);
	}
}

void USoulAnimInstance::PlayStun()
{
	if (StunMontage)
	{
		Montage_Play(StunMontage, 1.f);
	}
}

void USoulAnimInstance::PlayGuard()
{
	if (GuardMontage)
	{
		Montage_Play(GuardMontage, 1.f);
	}
}

//Locomotion 제어 함수
void USoulAnimInstance::OnPivotTurn()
{
	ASoulCharacter* Character = Cast<ASoulCharacter>(GetOwningActor());
	if (!Character) return;


	FVector Velocity = Character->GetVelocity();
	float Speed = Velocity.Size();
	FVector VelocityDir = Velocity.GetSafeNormal2D();

	FVector Input = Character->GetLastMovementInputVector(); //입력을 캐릭터가 따로 알고 있는듯 하다.

	FVector InputDir = Input.GetSafeNormal2D();


	float Dot = FVector::DotProduct(VelocityDir, InputDir);

	if (Speed > 200.f && Dot < -0.7f)
	{
		SavedPivotForward = Character->GetActorForwardVector();

		//Character->GetCharacterMovement()->bOrientRotationToMovement = false; 
		//애님인스턴스가 루트모션모드를 Root Motion from Everything으로 설정해서 필요 없어짐
		IsPivotTurn = true;
	}
	else
	{
		IsPivotTurn = false;
	}
	
}

void USoulAnimInstance::OnStrafe()
{
	ASoulCharacter* Character = Cast<ASoulCharacter>(GetOwningActor());
	if (!Character) return;


	FVector Forward = Character->GetActorForwardVector();
	FVector Right = Character->GetActorRightVector();

	FVector Input = Character->GetLastMovementInputVector().GetSafeNormal();

	ForwardInputValue = FVector::DotProduct(Input, Forward);
	RightInputValue = FVector::DotProduct(Input, Right);

	//Character->GetCharacterMovement()->bOrientRotationToMovement = false;
}
