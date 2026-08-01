// Fill out your copyright notice in the Description page of Project Settings.


#include "GAS/SoulGA_BasicAttack.h"
#include "AbilitySystemComponent.h"
#include "SoulAttributeSet.h"


void USoulGA_BasicAttack::ComboAttackCheck()
{
	UAbilitySystemComponent* AbilitySystemComponent = GetActorInfo().AbilitySystemComponent.Get();

    const USoulAttributeSet* SoulAttributeSet = AbilitySystemComponent->GetSet<USoulAttributeSet>();

    if (!SoulAttributeSet)
    {
        return;
    }

    float SP = SoulAttributeSet->GetSP();
    int Combo = SoulAttributeSet->GetComboCheck();


    /*

    if (SP >= 20.f && Combo < 3 && OnPressed)
    {
        SoulAttributeSet.ApplyGameplayEffectToSelf();


        SoulAttributeSet->ApplyGameplayEffectToSelf();
    }*/




	
}
