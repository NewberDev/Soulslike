// Fill out your copyright notice in the Description page of Project Settings.

/*
	소울류 게임에서 기본적으로 Hp와 대미지 받기를 구현하기 위해 기본적으로
	구현하는 부모 클래스이다.
*/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacterBase.generated.h"

UCLASS()
class SOULSLIKE_API AEnemyCharacterBase : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing = OnRep_CurrentHP, EditAnywhere, BlueprintReadWrite, Category = "State", Meta = (AllowPrivateAccess = true))
	float CurrentHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", Meta = (AllowPrivateAccess = true))
	float MaxHp;

public:
	// Sets default values for this character's properties
	AEnemyCharacterBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	//HP 변경의 구성
	void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	UFUNCTION()
	void OnRep_CurrentHP();

	//대미지를 받을시 실행되는 함수
	UFUNCTION()
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCaser
	)override;
};
