// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "NativeGameplayTags.h"

#include "SoulAbilitySystemComponent.generated.h"

/**
 * 
 */
UCLASS()
class SOULSLIKE_API USoulAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()


public:

	USoulAbilitySystemComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//IA의 정보를 InputTag로 받아올 함수.
	void AbilityInputTagPressed(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);


	void ProcessAbilityInput(float DeltaTime, bool bGamePaused);
	/* 매프레임 반복 실행으로 InputTag의 Handler로 TryActivateAbility 실행
	   하도록 만든건 맞지만 역시 언리얼 실행주기에서 자동실행하는게 아니기에
	   직접 Pawn, Controller, Component 등의 Tick에서 실행해줘야 한다.
	*/
	//bGamePaused는 게임이 일시정지 상태인지 확인하는 변수이다.

	void ClearAbilityInput(); 
	//모든 Tag가 중지돼야할때 Handle를 전부 Reset하는 용도

protected:
	// Handles to abilities that had their input pressed this frame.
	TArray<FGameplayAbilitySpecHandle> InputPressedSpecHandles;

	// Handles to abilities that had their input released this frame.
	TArray<FGameplayAbilitySpecHandle> InputReleasedSpecHandles;

	// Handles to abilities that have their input held.
	TArray<FGameplayAbilitySpecHandle> InputHeldSpecHandles;

	// Number of abilities running in each activation group.
	//int32 ActivationGroupCounts[(uint8)ESoulAbilityActivationGroup::MAX];
	//GameplayAbility에서 만들어야할 enum으로 추후에 추가하기로 한다.
};
