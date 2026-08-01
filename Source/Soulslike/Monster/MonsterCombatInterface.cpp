// Fill out your copyright notice in the Description page of Project Settings.


#include "Monster/MonsterCombatInterface.h"
#include "Kismet/GameplayStatics.h"

// Add default functionality here for any IMonsterCombatInterface functions that are not pure virtual.


// BlueprintNativeEvent의 진짜 알맹이는 뒤에 _Implementation을 붙여서 구현합니다.

/*
* 이 인터페이스는 공부용으로 만든 것으로 실 사용을 안하는 클래스이다.
* 블루프린트 연동 방법을 공부하는 것에만 초점을 맞추자!
*/

void IMonsterCombatInterface::SendDamage_Implementation(AActor* TargetActor, float DamageAmount)
{
	UE_LOG(LogTemp, Warning, TEXT("IMonsterCombatInterface::SendDamage_Implementation"));

	AActor* This = Cast<AActor>(this);
	if (!This || !TargetActor) return;

	UGameplayStatics::ApplyDamage(
		TargetActor, 
		DamageAmount, 
		Cast<APawn>(this)->GetController(), 
		This,
		UDamageType::StaticClass());

	UE_LOG(LogTemp, Log, TEXT(" [SendDamage] %s -> %s, Damage : %f."), *This->GetName(), *TargetActor->GetName(), DamageAmount);
}
