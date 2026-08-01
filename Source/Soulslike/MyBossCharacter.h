// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "NiagaraComponent.h"
#include "MyBossCharacter.generated.h"


class ABossProjectile;

UCLASS()
class SOULSLIKE_API AMyBossCharacter : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(ReplicatedUsing = OnRep_BossCurrentHP, EditAnywhere, BlueprintReadWrite, Category = "State", Meta = (AllowPrivateAccess = true))
	float BossCurrentHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", Meta = (AllowPrivateAccess = true))
	float BossMaxHp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State", Meta = (AllowPrivateAccess = true))
	UNiagaraComponent* MagicImpact;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	float AttackRange;
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Attack", Meta = (AllowPrivateAccess = true))
	float AttackRadius;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ABossProjectile> SpawnActorClass;

	

public:
	// Sets default values for this character's properties
	AMyBossCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//이 인스턴스의 모든 컴포넌트가 다 생성된 이후 + 초기화 후에 한번 실행되는 함수! -> 델리게이트 등록 좋음!
	virtual void PostInitializeComponents() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void GetLifetimeReplicatedProps(
		TArray<FLifetimeProperty>& OutLifetimeProps
	) const override;

	UFUNCTION()
	void OnRep_BossCurrentHP();

	UFUNCTION()
	virtual float TakeDamage(
		float DamageAmount,
		FDamageEvent const& DamageEvent,
		AController* EventInstigator,
		AActor* DamageCaser
	)override;

	void MagicSpawn();

	void CheckHpState();

	void CheckDeadState();

	void AttackHit1Check();

	void AttackHit2Check();

	void AOEHitCheck();

	void GrabCheck();

	void EenmyCheck(AController* EventInstigator);

	void MagicMontagePlaying(float DeltaTime);

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_MagicImpactDeactivate();

};
