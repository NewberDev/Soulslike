// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MonsterCombatInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UMonsterCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *  공격 기능만을 담당하는 인터페이스!
 */
class SOULSLIKE_API IMonsterCombatInterface
{
	GENERATED_BODY()


	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/**
	 * [블루프린트 호환형] 공격/피격 함수
	 * @param TargetActor : 공격하는 액터
	 * @param DamageAmount : 받은 대미지
	 */
	// [공격 신호 발생]
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "CombatSystem")
	void SendDamage(AActor* TargetActor, float DamageAmount);

	
	virtual void SendDamage_Implementation(
		AActor* TargetActor, 
		float DamageAmount);

};
