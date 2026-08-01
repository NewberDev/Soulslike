// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"

#include "SoulAttributeSet.generated.h"

/*
	캐릭터의 기본 동작이나, HP, SP, MP 등의 상태를
	조절하는 어트리뷰트를 관리한다.
*/

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_MULTICAST_DELEGATE(FOnHPChangedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnHPZeroDelegate);

/**
 * 
 */
UCLASS()
class SOULSLIKE_API USoulAttributeSet : public UAttributeSet
{
	GENERATED_BODY()


	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	FGameplayAttributeData WeaponState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	FGameplayAttributeData HP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	FGameplayAttributeData SP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	FGameplayAttributeData MaxSP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	FGameplayAttributeData MaxHP;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	FGameplayAttributeData ComboCheck;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	FGameplayAttributeData ATK;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	FGameplayAttributeData DEF;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Attribute")
	FGameplayAttributeData PotionCount;


	ATTRIBUTE_ACCESSORS(USoulAttributeSet, WeaponState);
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, HP);
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, SP);
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, MaxSP);
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, MaxHP);
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, ComboCheck);
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, ATK);
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, DEF);
	ATTRIBUTE_ACCESSORS(USoulAttributeSet, PotionCount);


	FOnHPZeroDelegate OnHPZeroDelegate;
	FOnHPChangedDelegate OnHPChangedDelegate;


	/* 어트리뷰트 값이 변하기 전에 실행되는 함수.
	* 용도: 변화 값만을 받아오기 때문에 값을 제어하는 로직을 담당한다.(수치 방어 전용)
	*/
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	/* 게임플레이 이펙트가 어트리뷰트에 적용된 이후에 실행되는 함수.
	* 용도: 변하는 값뿐만 아니라 가해자(Instigator)의 정보도 같이 따르기에 
	* 누가 보낸 이펙트인지 확인하고 UI 띄우기, 사운드 재생하기, 사망 처리하기를 처리한다. (게임 로직 처리 전용)
	*/
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
};
