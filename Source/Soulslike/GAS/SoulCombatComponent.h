// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SoulCombatComponent.generated.h"


/*
	현재 클래스는 캐릭터의 공격할때 실행할 ApplyDamage를 
	구현하는 컴포넌트이다.
*/
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SOULSLIKE_API USoulCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	float AttackRadius;
	float AttackRange;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "GameplayAbility", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayAbility> GA_Death;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, category = "GameplayEffect", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> DamageGameplayEffectClass;

	

public:	
	// Sets default values for this component's properties
	USoulCombatComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	//void AttackCheck();

	//void DamageCheck(float Damage);

	void GiveGA_Death();

	void TryGA_Death();

	UFUNCTION(BlueprintCallable)
	void LockOn_Enemy(AActor* Enemy, float DeltaTime);




		
};
