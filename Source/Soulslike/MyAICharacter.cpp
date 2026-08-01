// Fill out your copyright notice in the Description page of Project Settings.


#include "MyAICharacter.h"
#include "AIAnimInstance.h"
#include "DrawDebugHelpers.h" //12/1 - 디버깅 드로잉을 위한 헤더
#include "Kismet/GameplayStatics.h" //12/3 - 데이지 주기
#include "Net/UnrealNetwork.h" //Replication 구현을 위한 헤더

// Sets default values
AMyAICharacter::AMyAICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	AttackRange = 300.f;
	AttackRadius = 50.f;

	CurrentHp = 20;
}

// Called when the game starts or when spawned
void AMyAICharacter::BeginPlay()
{
	Super::BeginPlay();

	
	
}

// Called every frame
void AMyAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyAICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMyAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UAIAnimInstance* AnimInstance = Cast<UAIAnimInstance>(GetMesh()->GetAnimInstance());
	if (AnimInstance)
	{
		AnimInstance->OnAttackHitCheckDelegate.AddUObject(this, &AMyAICharacter::AttackCheck);
	}
}

void AMyAICharacter::AttackCheck()
{
	UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck")); //함수 실행 확인
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel( //레이저 쏘는 함수
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	if (bResult)
	{
		UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck - Something Hit"));
		AActor* HitActor = HitResult.GetActor(); //부딪힌 액터의 정보를 가져온다.
		/*
		if (IsValid(HitActor))
		{
			UE_LOG(LogTemp, Log, TEXT("Hit Actor Name : %s"), *HitActor->GetName());
		}
		//12/3 - 대미지 주기 - 아래의 함수가 실행되면 그값이 TakeDamage의 파라미터인 DamageAmount 값으로 전달된다.
		*/
		UGameplayStatics::ApplyDamage(
			HitActor, //딜 대상
			1.0f, // 딜 수치
			GetInstigatorController(),
			this, //딜 주는 대상
			UDamageType::StaticClass() //딜의 종류
		);
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck - No Hit"));
	}




	//12/1 - 디버깅 드로잉 : 아래의 사용법을 잘 공부해보자!
#if ENABLE_DRAW_DEBUG
	FVector TracVec = GetActorForwardVector() * AttackRange; //길이를 얼마나 할것인가? 플레이어가 바라보는 방향 * Range
	FVector Center = GetActorLocation() + TracVec * 0.5f; // 중심 위치 TracVec의 시작과 끝의 중심으로
	float HalfHeight = AttackRange * 0.5f + AttackRadius; // 절반의 길이
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TracVec).ToQuat(); //바라보는 방향으로 회전
	//회전 방식은 1. Quatunion(다차원 벡터로 회전) / 2. Euler(각으로 회전) 
	// : 1번이 이해가 더 어렵지만, Euler의 각도에서는 짐벌락?
	FColor DrawColor = bResult ? FColor::Green : FColor::Red;
	float DebugLifeTime = 5.0f;

	DrawDebugCapsule(
		GetWorld(), //월드
		Center,  //시작지점
		HalfHeight, //캡슐의 절반 높이 (중심을 기준으로 보기에 절반만 보는것이다.)
		AttackRadius, //반지름
		CapsuleRot, //회전값
		DrawColor, //색상 지정 - 지금 코드는 액터에 닿으면, 닿지 않으면 색이 다름
		false, //기본 false로, true이면 도형이 프레임당 업데이트 되지 않아도 남아있게 해주는 옵션
		DebugLifeTime //디버깅 유지 시간을 말함.
	);
#endif
}


void AMyAICharacter::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyAICharacter, CurrentHp); //이 표현으로 결국 이 변수는 실행동안 리플리케이션 된다.

}

void AMyAICharacter::DoRagdoll()
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SkeletalMesh->SetSimulatePhysics(true);

}

float AMyAICharacter::TakeDamage(
	float DamageAmount,
	FDamageEvent const& DamageEvent,
	AController* EventInstigator,
	AActor* DamageCaser
)
{
	float Damage = Super::TakeDamage(
		DamageAmount,
		DamageEvent,
		EventInstigator,
		DamageCaser
	);
	UAIAnimInstance* AnimInstance = Cast<UAIAnimInstance>(GetMesh()->GetAnimInstance());
	if (!AnimInstance->IsRollMontagePlaying())
	{
		CurrentHp -= Damage;
	}

	ServerRPC_CurrentHpCheck();

	//UE_LOG(LogTemp, Log, TEXT("Enemy CurrentHealth: %f"), CurrentHp);


	if (CurrentHp <= 0)
	{
		//래그돌로 바꾸기
		Multicast_Roll_Implementation();
	}
	return Damage;
}

void AMyAICharacter::ServerRPC_CurrentHpCheck_Implementation()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f, FColor::Red,
		FString::Printf(TEXT("Enemy CurrentMP: %f"), CurrentHp)
	);
}

void AMyAICharacter::Multicast_Roll_Implementation()
{
	DoRagdoll();
}
