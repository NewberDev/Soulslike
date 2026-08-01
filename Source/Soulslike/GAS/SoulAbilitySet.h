// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "GameplayAbilitySpecHandle.h"
#include "SoulAbilitySet.generated.h"

class UAbilitySystemComponent;
class USoulGameplayAbility;
class USoulAbilitySystemComponent;

/**
 * 
 */

USTRUCT(BlueprintType)
struct FSoulAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:

	// Gameplay ability to grant.
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USoulGameplayAbility> Ability = nullptr;

	// Level of ability to grant.
	UPROPERTY(EditDefaultsOnly)
	int32 AbilityLevel = 1;

	// Tag used to process input for the ability.
	UPROPERTY(EditDefaultsOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/*
	이 AbilitySet의 역할은 단순히 Ability와 InputTag를 짝짓는것을 넘어
	어트리뷰트와 게임플레이 이펙트를 저장하고 이것들을 관리하는 역할도
	하지만 지금은 잠시 보류한다. 그렇기에 아래의 Handle구조체도
	추후에 어트리뷰트와 이펙트 부분을 추가해야 한다.
*/

USTRUCT(BlueprintType)
struct FSoulAbilitySet_GrantedHandles
{
	GENERATED_BODY()

public:

	void AddAbilitySpecHandle(const FGameplayAbilitySpecHandle& Handle);

protected:

	// Handles to the granted abilities.
	UPROPERTY()
	TArray<FGameplayAbilitySpecHandle> AbilitySpecHandles;
	//이 구조체는 추후 GA, GE, Attribute가 제거할때 사용하기 위한 기록용 핸들러이다.
};

/*
	DataAsset이 단순히 정보를 저장하는 기능만을 가진 클래스라면,
	PrimaryDataAsset은 저장을 넘어, 엔진이 관리(Asset Manager)하는
	독립적인 리소스 클래스라고 할수 있다.
*/
UCLASS()
class SOULSLIKE_API USoulAbilitySet : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:

	USoulAbilitySet(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void GiveToAbilitySystem(UAbilitySystemComponent* SoulASC, FSoulAbilitySet_GrantedHandles* OutGrantedHandles, UObject* SourceObject = nullptr) const;
	//GA를 등록시키는 함수이다.

protected:

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay Abilities", meta = (TitleProperty = Ability))
	TArray<FSoulAbilitySet_GameplayAbility> GrantedGameplayAbilities;
	
};
