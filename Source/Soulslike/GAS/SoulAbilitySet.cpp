// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulAbilitySet.h"
#include "SoulGameplayAbility.h"
#include "SoulAbilitySystemComponent.h"



void FSoulAbilitySet_GrantedHandles::AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle)
{
	if (Handle.IsValid())
	{
		AbilitySpecHandles.Add(Handle); // 나중에 제거하기 위한 기록용이다.
	}
}


USoulAbilitySet::USoulAbilitySet(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{

}


//이 동작 원리에 주의 할것!
void USoulAbilitySet::GiveToAbilitySystem(UAbilitySystemComponent* SoulASC, FSoulAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject) const
{
	check(SoulASC);

	//if (!(SoulASC->IsOwnerActorAuthoritative())) return;

	for (int32 AbilityIndex = 0; AbilityIndex < GrantedGameplayAbilities.Num(); ++AbilityIndex)
	{
		const FSoulAbilitySet_GameplayAbility& AbilityToGrant = GrantedGameplayAbilities[AbilityIndex];

		if (!IsValid(AbilityToGrant.Ability))
		{
			UE_LOG(LogTemp, Error, TEXT("GrantedGameplayAbilities[%d] on ability set [%s] is not valid."), AbilityIndex, *GetNameSafe(this));
			continue;
		}

		USoulGameplayAbility* AbilityCDO = AbilityToGrant.Ability->GetDefaultObject<USoulGameplayAbility>();

		FGameplayAbilitySpec AbilitySpec(AbilityCDO, AbilityToGrant.AbilityLevel);
		AbilitySpec.SourceObject = SourceObject;
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityToGrant.InputTag);

		const FGameplayAbilitySpecHandle AbilitySpecHandle = SoulASC->GiveAbility(AbilitySpec);
	}

	/*
		반복문을 통해, 어빌리티와 InputTag 등의 정보가 Spec으로 합쳐지고
		ASC의 GiveAbility로 인해 등록된다. 
	*/
}
