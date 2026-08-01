// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyCharacterBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AEnemyCharacterBase::AEnemyCharacterBase()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentHp = 10;
	MaxHp = CurrentHp;

}

// Called when the game starts or when spawned
void AEnemyCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AEnemyCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemyCharacterBase::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//HP 변경 구성
void AEnemyCharacterBase::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AEnemyCharacterBase, CurrentHp); //이 표현으로 결국 이 변수는 실행동안 리플리케이션 된다.

}

void AEnemyCharacterBase::OnRep_CurrentHP()
{

	UE_LOG(LogTemp, Log, TEXT("AMyBossCharacter::OnRep_BossCurrentHP"));
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f, FColor::Green,
		FString::Printf(TEXT("BossCurrentMP: %f"), CurrentHp)
	);
}


/*  부모의 TakeDamage는 들어온 대미지 만큼을 CurrentHp에서 감소하는 목적이다.
	자식클래스에서는 Spuer::TakeDamage에다가 대미지 값을 방어력만큼 줄이거나,
	방어의 경우 0으로 고정하는 등의 조정하여 전달만 하면 현재 EnemyCharacterBase
	클래스가 대미지 감소를 담당한다.
*/
float AEnemyCharacterBase::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCaser
)
{
	//이 Super::--- 코드는 바로위의 부모(그 위는 아님)을 호출하는 기능이다.
	// 그렇기에 이것에 의해 자식이 TakeDamage가 실행될때 같은게 두번 실행되진 않는다.
	Super::TakeDamage(
		DamageAmount,
		DamageEvent,
		EventInstigator,
		DamageCaser
	);

	CurrentHp -= DamageAmount;

	if (DamageAmount != 0)
	{
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f, FColor::Red,
			FString::Printf(TEXT("BossCurrentHP: %f"), CurrentHp)
		);
	}
		
	return DamageAmount;
}

