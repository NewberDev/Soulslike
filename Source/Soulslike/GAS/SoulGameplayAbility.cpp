// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulGameplayAbility.h"
#include "SoulAbilitySystemComponent.h"
#include "SoulGameplayTags.h"

USoulGameplayAbility::USoulGameplayAbility()
{
	OnPressed = false;
}

void USoulGameplayAbility::InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	//UE_LOG(LogTemp, Warning, TEXT("USoulGameplayAbility::InputPressed"));
	OnPressed = true;
}

void USoulGameplayAbility::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputReleased(Handle, ActorInfo, ActivationInfo);

	//UE_LOG(LogTemp, Warning, TEXT("USoulGameplayAbility::InputReleased"));
	OnPressed = false;

	UAbilitySystemComponent* ASC = ActorInfo->AbilitySystemComponent.Get();
	if (!ASC) return;

	
	if (ASC->HasMatchingGameplayTag(SoulGameplayTags::Gameplay_State_Action_OnShield))
	{
		if (UAnimInstance* AnimInstance = ActorInfo->GetAnimInstance())
		{
			AnimInstance->Montage_Stop(0.2f); // 부드럽게 끊기 위해 조금의 텀을 둔다.
		}

		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
	}
	

}
