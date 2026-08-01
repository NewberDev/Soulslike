// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "ComboAttackAttributeSet.generated.h"


#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
//이 매크로 함수를 위해서 헤더 "AbilitySystemComponent.h"가 필요하다

/**
 * 
 */
UCLASS()
class SOULSLIKE_API UComboAttackAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category="Attribute")
	FGameplayAttributeData HasNextComboInput;
	//게임 어트리뷰트 데이터로 만들어진 값은 전부 기본 float형이다.
	//bool로 생각하기 위해서 0일때 false, 0이 아닐?? true로 사용한다.

	/*
	UFUNCTION(BlueprintCallable)
	void SetHasNextComboInput(float value);
	*/
	/*   
		매크로 함수로 게터, 세터를 설정했기에 필요 없어진다.
		그리고 어트리뷰트는 이렇게 함수로 직접 접근해서 사용하는건 권장하지 않는 방법이다.
		권장하는 방법은 게임플레이 이펙트를 이용하는 접근이다.
	*/

	/* //어트리뷰트셋의 변수를 활용하기 위함 -> 귀찮기에 위에서 한번에 매크로 함수로 선언한다.
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(UComboAttackAttributeSet, HasNextComboInput)
	GAMEPLAYATTRIBUTE_VALUE_GATTER(HasNextComboInput)
	GAMEPLAYATTRIBUTE_VALUE_SETTER(HasNextComboInput)
	GAMEPLAYATTRIBUTE_VALUE_INITTER(HasNextComboInput)
	*/


	//#define에 의해 위쪽에 긴 메크로들이 간단하게 정리할수 있다.
	ATTRIBUTE_ACCESSORS(UComboAttackAttributeSet, HasNextComboInput)
};
