// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulAbilitySystemComponent.h"
#include "SoulGameplayAbility.h"

USoulAbilitySystemComponent::USoulAbilitySystemComponent(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}

void USoulAbilitySystemComponent::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	//UE_LOG(LogTemp, Warning, TEXT("InputTag Pressed: %s"), *InputTag.ToString());

	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				InputPressedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.AddUnique(AbilitySpec.Handle);
			}
		}
	}
}

void USoulAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{

	if (InputTag.IsValid())
	{
		for (const FGameplayAbilitySpec& AbilitySpec : ActivatableAbilities.Items)
		{
			if (AbilitySpec.Ability && (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag)))
			{
				InputReleasedSpecHandles.AddUnique(AbilitySpec.Handle);
				InputHeldSpecHandles.Remove(AbilitySpec.Handle);
			}
		}
	}
}

void USoulAbilitySystemComponent::ProcessAbilityInput(float DeltaTime, bool bGamePaused)
{

	//UE_LOG(LogTemp, Warning, TEXT("PressedHandles Num: %d"), InputPressedSpecHandles.Num());
	TArray<FGameplayAbilitySpecHandle> AbilitiesToActivate; 
	//위 배열은 Lyra에서 static 구현이지만, 내 직접 만든 코드는 불완전하기에 배제한다.
	AbilitiesToActivate.Reset();

	//Held의 코드는 보류한다.
	
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputPressedSpecHandles)
	{
		/*
			AbilitySpecHandle은 어빌리티의 ID 역할이고, 
			AbilitySpec은 실제 어빌리티의 실행정보를 담고 있다.
		*/
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle)) 
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = true;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputPressed(*AbilitySpec);
				}
				else
				{
					const USoulGameplayAbility* SoulAbilityCDO = Cast<USoulGameplayAbility>(AbilitySpec->Ability);

					if (SoulAbilityCDO)
					{
						AbilitiesToActivate.AddUnique(AbilitySpec->Handle);
					}
					/*
						LyraAbilityCDO->GetActivationPolicy() == ELyraAbilityActivationPolicy::OnInputTriggered
						조건문에 이와 같은 실행정책을 확인하는 코드가 Lyra에는 있다.
						이것으로 눌렸을때, 누르고 있을때를 구분 실행할수 있으나,
						지금은 패스한다. --> 추후 공부할것!
					*/
				}
			}
		}
	}
	/*
	bool bIsAnyAbilityRunning = false;
	for (const FGameplayAbilitySpec& Spec : ActivatableAbilities.Items)
	{
		if (Spec.IsActive())
		{
			// 하나라도 실행 중인 게 있다면 플래그를 true로
			bIsAnyAbilityRunning = true;
			break;
		}
	}
	*/

	//위에서 확인한 AbilitySpec의 정보로 어빌리티를 실행한다.
	for (const FGameplayAbilitySpecHandle& AbilitySpecHandle : AbilitiesToActivate)
	{
		//if (bIsAnyAbilityRunning) break;
		UE_LOG(LogTemp, Warning, TEXT("TryActivateAbility Called"));
		TryActivateAbility(AbilitySpecHandle);
	}


	//실행이후에, AbilitySpec의 InputPressed를 false로 바꾼다.
	for (const FGameplayAbilitySpecHandle& SpecHandle : InputReleasedSpecHandles)
	{
		if (FGameplayAbilitySpec* AbilitySpec = FindAbilitySpecFromHandle(SpecHandle))
		{
			if (AbilitySpec->Ability)
			{
				AbilitySpec->InputPressed = false;

				if (AbilitySpec->IsActive())
				{
					// Ability is active so pass along the input event.
					AbilitySpecInputReleased(*AbilitySpec);
				}
			}
		}
	}

	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
}

void USoulAbilitySystemComponent::ClearAbilityInput()
{
	InputPressedSpecHandles.Reset();
	InputReleasedSpecHandles.Reset();
	InputHeldSpecHandles.Reset();
}
