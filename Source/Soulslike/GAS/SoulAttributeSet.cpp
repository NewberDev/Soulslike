
// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulAttributeSet.h"
#include "AbilitySystemComponent.h"

#include "GameplayEffectExtension.h" //FGameplayEffectModCallbackData를 위한 헤더



void USoulAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{ 
	Super::PreAttributeChange(Attribute, NewValue);

	/*
	if (Attribute == GetHPAttribute())
	{
		const float Max = GetMaxHP();
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHP());
	}*/
}

void USoulAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	
	if (Data.EvaluatedData.Attribute == GetSPAttribute())
	{
		const float Max = GetMaxSP();
		SetSP(FMath::Clamp(GetSP(), 0.f, Max));
	}


	if (Data.EvaluatedData.Attribute == GetHPAttribute())
	{
		// 방어력 만큼 대미지 차감 로직.
		float AppliedModifier = Data.EvaluatedData.Magnitude;
		

		if (AppliedModifier <= 0.f) // 체력이 깎이는 상황(대미지)일 때만 방어력 적용
		{
			// 2. 타겟(나 자신)의 AbilitySystemComponent를 가져온다.
			UAbilitySystemComponent* TargetASC = &Data.Target;

			// 3. 무적 태그를 가지고 있는지 검사
			// FGameplayTag::RequestGameplayTag로 태그를 동적으로 찾거나, 이미 선언된 태그를 가져온다.
			FGameplayTag InvincibilityTag = FGameplayTag::RequestGameplayTag(FName("GameplayEffect.Buff.Invincibility"));

			if (TargetASC && TargetASC->HasMatchingGameplayTag(InvincibilityTag))
			{
				// 대미지를 0으로 만든다는 것은 이미 깎인 HP를 원래대로 복구시킨다는 의미이다.
				// AppliedModifier가 음수이므로, 절댓값만큼 다시 더해준다.
				SetHP(GetHP() + FMath::Abs(AppliedModifier));

				// 밑에 진행하는 로직(방어력 계산, Clamp, 델리게이트 등)을 모두 무시하고 종료
				return;
			}

			float RawDamage = FMath::Abs(AppliedModifier);
			float CurrentDEF = GetDEF();

			// 최종 대미지 계산 (최소 0)
			float FinalDamage = FMath::Max(RawDamage - CurrentDEF, 0.f);

			// 방어력으로 뎀감을 해준 차액 계산 (예: 원본 50 - 최종 30 = 뎀감량 20)
			float DamageMitigated = RawDamage - FinalDamage;

			// 이미 엔진이 원본 대미지만큼 깎아놓은 HP에 뎀감량만큼을 다시 더해줘서 보정 ******
			SetHP(GetHP() + DamageMitigated);

			if (GetHP() > 0.f)
			{
				OnHPChangedDelegate.Broadcast();
			}
		}


		const float Max = GetMaxHP();
		SetHP(FMath::Clamp(GetHP(), 0.f, Max));

		

		if (GetHP() <= 0.f)
		{
			UE_LOG(LogTemp, Warning, TEXT("USoulAttributeSet::PostGameplayEffectExecute_ GetHP() <= 0"));
			OnHPZeroDelegate.Broadcast();
		}
	}
	
}