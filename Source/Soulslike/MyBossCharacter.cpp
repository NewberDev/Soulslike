// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBossCharacter.h"
#include "Net/UnrealNetwork.h"
#include "MyBossAnimInstance.h"
#include "Kismet/GameplayStatics.h" //데이지 주기 
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h" //블랙보드 접근
#include "BossProjectile.h"
#include "Kismet/KismetMathLibrary.h"
#include "GAS/SoulCharacter.h"
#include "GAS/SoulHUD.h"

// Sets default values
AMyBossCharacter::AMyBossCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BossCurrentHp = 40.f;
	BossMaxHp = 40.f;

	MagicImpact = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MagicImpact"));


	MagicImpact->SetupAttachment(GetMesh());
	MagicImpact->bAutoActivate = false;
	

	AttackRange = 300.f;
	AttackRadius = 50.f;

	

}

// Called when the game starts or when spawned
void AMyBossCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC) return;

	ASoulHUD* HUD = Cast<ASoulHUD>(PC->GetHUD());
	if (!HUD) return;

	HUD->SetBoss(this);
}

void AMyBossCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UMyBossAnimInstance* AnimInstance = Cast<UMyBossAnimInstance>(GetMesh()->GetAnimInstance());

	if (AnimInstance) //성공적으로 받아왔을때,
	{
		if (HasAuthority())
		{
			AnimInstance->OnMagicSpawnDelegate.AddUObject(this, &AMyBossCharacter::MagicSpawn);
		}
		//우리가 만드는 DECLARE_MULTICAST_DELEGATE 선언 델리게이트엔 AddDynamic이 없다
		//AddUObject로 써야 한다.

		AnimInstance->OnAttackHit1Delegate.AddUObject(this, &AMyBossCharacter::AttackHit1Check);
		AnimInstance->OnAttackHit2Delegate.AddUObject(this, &AMyBossCharacter::AttackHit2Check);
		AnimInstance->OnAOEHitDelegate.AddUObject(this, &AMyBossCharacter::AOEHitCheck);
		AnimInstance->OnGrabDelegate.AddUObject(this, &AMyBossCharacter::GrabCheck);
	}
}

// Called every frame
void AMyBossCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	 
	//MagicMontagePlaying(DeltaTime);
}

// Called to bind functionality to input
void AMyBossCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AMyBossCharacter::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyBossCharacter, BossCurrentHp); //이 표현으로 결국 이 변수는 실행동안 리플리케이션 된다.

}

void AMyBossCharacter::OnRep_BossCurrentHP()
{
	
	UE_LOG(LogTemp, Log, TEXT("AMyBossCharacter::OnRep_BossCurrentHP"));
	/*
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f, FColor::Green,
		FString::Printf(TEXT("BossCurrentMP: %f"), BossCurrentHp)
	);
	*/
}


float AMyBossCharacter::TakeDamage(
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


	//BossCurrentHp -= Damage;
	//OnRep_BossCurrentHP();
	///방어시 대미지 피하기!
	UMyBossAnimInstance* AnimInstance = Cast<UMyBossAnimInstance>(GetMesh()->GetAnimInstance());
	if (!(AnimInstance->IsGuardMontagePlaying()))
	{
		BossCurrentHp -= Damage;
		/*
		GEngine->AddOnScreenDebugMessage(
			-1,
			5.f, FColor::Red,
			FString::Printf(TEXT("BossCurrentHP: %f"), BossCurrentHp)
		);
		*/
		//EenmyCheck(EventInstigator);
	}

	//AIController BP → OnPossess → RunBehaviorTree() 이기 때문에 바로 블랙보드에 접근가능하다.
	if (BossCurrentHp / BossMaxHp < 0.5)
	{
		CheckHpState();
	}
	if (BossCurrentHp <= 0)
	{
		CheckDeadState();

		APlayerController* PC = GetWorld()->GetFirstPlayerController();
		if (!PC) return 0;

		ASoulHUD* HUD = Cast<ASoulHUD>(PC->GetHUD());
		if (!HUD) return 0;

		HUD->RemoveUI(HUD->BossHPWidgetInstance);
	}
	return Damage;
}

void AMyBossCharacter::EenmyCheck(AController* EventInstigator)
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (!AIC) return;

	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB) return;

	APawn* InstigatorPawn = EventInstigator->GetPawn();
	if (!InstigatorPawn) return;

	if (!(BB->GetValueAsBool(TEXT("HasLineOfSight"))))
	{
		BB->SetValueAsObject(TEXT("EnemyActor"), InstigatorPawn);
		BB->SetValueAsBool(TEXT("HasLineOfSight"), true);
	}
	
}

void AMyBossCharacter::CheckHpState()
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (!AIC) return;

	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB) return;

	BB->SetValueAsBool(TEXT("AOE"), true);

}

void AMyBossCharacter::CheckDeadState()
{
	AAIController* AIC = Cast<AAIController>(GetController());
	if (!AIC) return;

	UBlackboardComponent* BB = AIC->GetBlackboardComponent();
	if (!BB) return;

	BB->SetValueAsBool(TEXT("BossDead"), true);
}

void AMyBossCharacter::MagicMontagePlaying(float DeltaTime)
{
	UMyBossAnimInstance* AnimInstance = Cast<UMyBossAnimInstance>(GetMesh()->GetAnimInstance());
	if (!AnimInstance)
	{
		//UE_LOG(LogTemp, Error, TEXT("AnimInstance NULL"));
		return;
	}
	
	if (AnimInstance->IsMagicMontagePlaying())
	{
		UE_LOG(LogTemp, Log, TEXT("AMyBossCharacter::MagicMontagePlaying"));
		AAIController* AIC = Cast<AAIController>(GetController());
		if (!AIC) return;

		UBlackboardComponent* BB = AIC->GetBlackboardComponent();
		if (!BB) return;

		AActor* Target =
			Cast<AActor>(BB->GetValueAsObject("EnemyActor"));
		//블랙보드에서 EnemyActor값을 가져온다.
		if (!Target) return;

		FRotator LookRot =
			UKismetMathLibrary::FindLookAtRotation(
				GetActorLocation(),
				Target->GetActorLocation()
			);
		//그 값에서 위치를 얻어내고,
		SetActorRotation(
			FMath::RInterpTo(GetActorRotation(), LookRot, DeltaTime, 5.f)
		);
		//EnemyActor의 방향으로 회전한다.
	}
}

void AMyBossCharacter::MagicSpawn()
{

	UE_LOG(LogTemp, Log, TEXT("AMyBossCharacter::MagicSpawn"));

	Multicast_MagicImpactDeactivate();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetActorLocation() + FVector(0.f, 0.f, 190.f));

	FRotator SpawnRotation = FRotator::ZeroRotator;

	//블랙보드가 가진 적대 캐릭터의 정보를 받아와서 발사체에게 전달하기!
	AAIController* AICon = Cast<AAIController>(Controller);

	ACharacter* Target = nullptr;

	if (AICon)
	{
		UBlackboardComponent* BB = AICon->GetBlackboardComponent();

		if (BB)
		{
			Target =
				Cast<ACharacter>(BB->GetValueAsObject(TEXT("EnemyActor")));
		}
	}

	//SpawnTransform.SetRotation(GetActorForwardVector().Rotation.Quaternion);
	if (Controller)
	{
		SpawnTransform.SetRotation(Controller->GetControlRotation().Quaternion());
	}
	SpawnTransform.SetScale3D(FVector(1.f));



	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ABossProjectile* Projectile1 = GetWorld()->SpawnActor<ABossProjectile>(
		SpawnActorClass,
		SpawnTransform,
		SpawnParams
	); 

	if (Projectile1)
	{
		Projectile1->TargetCharacter = Target;
	}

	
	SpawnTransform.SetLocation(GetActorLocation() + FVector(0.f, 0.f, 140.f) + GetActorRightVector() * 180.f);


	ABossProjectile* Projectile2 = GetWorld()->SpawnActor<ABossProjectile>(
		SpawnActorClass,
		SpawnTransform,
		SpawnParams
	);

	if (Projectile2)
	{
		Projectile2->TargetCharacter = Target;
	}

	SpawnTransform.SetLocation(GetActorLocation() + FVector(0.f, 0.f, 140.f) - GetActorRightVector() * 180.f);


	ABossProjectile* Projectile3 = GetWorld()->SpawnActor<ABossProjectile>(
		SpawnActorClass,
		SpawnTransform,
		SpawnParams
	);

	if (Projectile3)
	{
		Projectile3->TargetCharacter = Target;
	}
	
}

void AMyBossCharacter::Multicast_MagicImpactDeactivate_Implementation()
{
	MagicImpact->Deactivate();
}

void AMyBossCharacter::AttackHit1Check()
{
	//UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck")); //함수 실행 확인
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	FVector StartPoint = GetActorLocation() + GetActorRightVector() * 50.f + GetActorForwardVector()*200.f;
	FVector EndPoint = StartPoint - GetActorRightVector()*100.f + GetActorUpVector()*130.f;
	bool bResult = GetWorld()->SweepSingleByChannel( //레이저 쏘는 함수
		HitResult,
		StartPoint,
		EndPoint,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	if (bResult && HasAuthority()) //클라이언트에서 또 충돌체크 방지!
	{
		UE_LOG(LogTemp, Log, TEXT("ABossCharacter::AttackCheck - Something Hit"));
		AActor* HitActor = HitResult.GetActor(); //부딪힌 액터의 정보를 가져온다.
		if (IsValid(HitActor))
		{
			UE_LOG(LogTemp, Log, TEXT("Hit Actor Name : %s"), *HitActor->GetName());
		}
		//12/3 - 대미지 주기 - 아래의 함수가 실행되면 그값이 TakeDamage의 파라미터인 DamageAmount 값으로 전달된다.
		
		UGameplayStatics::ApplyDamage(
			HitActor,
			4.0f,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);

	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("ABossCharacter::AttackCheck - No Hit"));
	}



	/*
	//디버깅 드로잉 : 아래의 사용법을 잘 공부해보자!
#if ENABLE_DRAW_DEBUG
	FVector TracVec = EndPoint - StartPoint; //길이를 얼마나 할것인가? 플레이어가 바라보는 방향 * Range
	FVector Center = StartPoint + TracVec * 0.5f; // 중심 위치 TracVec의 시작과 끝의 중심으로
	float HalfHeight = AttackRange * 0.5f + AttackRadius; // 절반의 길이
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TracVec).ToQuat(); //바라보는 방향으로 회전
	//회전 방식은 1. Quatunion(다차원 벡터로 회전) / 2. Euler(각으로 회전) 
	// : 1번이 이해가 더 어렵지만, Euler의 각도에서는 짐벌락 문제 있음
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
	*/
}

void AMyBossCharacter::AttackHit2Check()
{
	//UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck")); //함수 실행 확인
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	FVector StartPoint = GetActorLocation() + GetActorRightVector() * 60.f + GetActorForwardVector() * 150.f;
	FVector EndPoint = StartPoint - GetActorRightVector() * 120.f ;
	bool bResult = GetWorld()->SweepSingleByChannel( //레이저 쏘는 함수
		HitResult,
		StartPoint,
		EndPoint,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	if (bResult && HasAuthority())
	{
		UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck - Something Hit"));
		AActor* HitActor = HitResult.GetActor(); //부딪힌 액터의 정보를 가져온다.
		if (IsValid(HitActor))
		{
			UE_LOG(LogTemp, Log, TEXT("Hit Actor Name : %s"), *HitActor->GetName());
		}
		//12/3 - 대미지 주기 - 아래의 함수가 실행되면 그값이 TakeDamage의 파라미터인 DamageAmount 값으로 전달된다.
		UGameplayStatics::ApplyDamage(
			HitActor,
			3.0f,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);

	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck - No Hit"));
	}



	/*
	//디버깅 드로잉 : 아래의 사용법을 잘 공부해보자!
#if ENABLE_DRAW_DEBUG
	FVector TracVec = EndPoint - StartPoint; //길이를 얼마나 할것인가? 플레이어가 바라보는 방향 * Range
	FVector Center = StartPoint + TracVec * 0.5f; // 중심 위치 TracVec의 시작과 끝의 중심으로
	float HalfHeight = AttackRange * 0.5f + AttackRadius; // 절반의 길이
	FQuat CapsuleRot = FRotationMatrix::MakeFromZ(TracVec).ToQuat(); //바라보는 방향으로 회전
	//회전 방식은 1. Quatunion(다차원 벡터로 회전) / 2. Euler(각으로 회전) 
	// : 1번이 이해가 더 어렵지만, Euler의 각도에서는 짐벌락 문제 있음
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
	*/
}

void AMyBossCharacter::AOEHitCheck()
{
	//UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck")); //함수 실행 확인
	//TArray<FHitResult> HitResults;
	//FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	TArray<FOverlapResult> OverlapResults;

	float Radius = 760.f;       // 원기둥의 반지름
	float HalfHeight = 5.f; //원기둥 높이

	//FCollisionShape CapsuleShape = FCollisionShape::MakeCapsule(Radius, HalfHeight);
	//원기둥 충돌체 만들기!
	FCollisionShape BoxShape = FCollisionShape::MakeBox(FVector(Radius, Radius, HalfHeight));
	FVector Center = GetActorLocation() + FVector(0.f, 0.f, -20.f);
	//FVector StartPoint = GetActorLocation();
	//FVector EndPoint = StartPoint - GetActorUpVector() * 40.f;
	
	bool bHasOverlap = GetWorld()->OverlapMultiByChannel( //레이저 쏘는 함수
		OverlapResults,
		Center,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		BoxShape,
		Params
	); //멀티로 설정하니까 뼈대 마다 맞아서 한번에 여러번 맞게 된다!

	//기존 싱글
	/*
	bool bResult = GetWorld()->SweepSingleByChannel( 
		HitResult,
		StartPoint,
		EndPoint,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel1,
		CapsuleShape,
		Params
	);
	*/
	if (bHasOverlap && HasAuthority())
	{

		TArray<AActor*> AlreadyHitActors; // 중복 대미지 방지 리스트
		
		//반복문을 통해 범위 내 모든 액터 처리
		for (const FOverlapResult& Result : OverlapResults) //C++11에서 등장한 방식으로 배열의 값을 임시로 받아서 반복하고 배열의 길이만큼 반복한다.
		{
			AActor* HitActor = Result.GetActor();
			if (IsValid(HitActor) && HitActor != this && !AlreadyHitActors.Contains(HitActor))
			{
				AlreadyHitActors.Add(HitActor);

				UE_LOG(LogTemp, Log, TEXT("적중 액터: %s"), *HitActor->GetName());

				UGameplayStatics::ApplyDamage(
					HitActor,
					20.0f,
					GetInstigatorController(),
					this,
					UDamageType::StaticClass()
				);
			}
		} 

		//기존 싱글
		/*
		UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck - Something Hit"));
		AActor* HitActor = HitResult.GetActor(); //부딪힌 액터의 정보를 가져온다.
		if (IsValid(HitActor))
		{
			UE_LOG(LogTemp, Log, TEXT("Hit Actor Name : %s"), *HitActor->GetName());
		}
		//12/3 - 대미지 주기 - 아래의 함수가 실행되면 그값이 TakeDamage의 파라미터인 DamageAmount 값으로 전달된다.
		UGameplayStatics::ApplyDamage(
			HitActor,
			2.0f,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);
		*/
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck - No Hit"));
	}

	//디버깅 드로잉 : 아래의 사용법을 잘 공부해보자!
	/*
#if ENABLE_DRAW_DEBUG
	FColor DrawColor = bHasOverlap ? FColor::Green : FColor::Red;
	*/
	/*
	DrawDebugCapsule(
		GetWorld(),
		StartPoint,
		HalfHeight,
		Radius,
		FQuat::Identity,
		DrawColor,
		false,
		2.0f
	);*/

	/*
	DrawDebugCylinder(GetWorld(), 
		Center + FVector(0, 0, HalfHeight), // 천장 (중심 + 절반높이)
		Center - FVector(0, 0, HalfHeight), // 바닥 (중심 - 절반높이) 
		Radius, 32, 
		DrawColor,
		false, 
		2.0f
	);
#endif */
	
}

void AMyBossCharacter::GrabCheck()
{
	//UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck")); //함수 실행 확인
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);

	FVector StartPoint = GetActorLocation() + GetActorRightVector() * 50.f + GetActorForwardVector() * 200.f;
	FVector EndPoint = StartPoint - GetActorRightVector() * 100.f + GetActorUpVector() * 130.f;
	bool bResult = GetWorld()->SweepSingleByChannel( //레이저 쏘는 함수
		HitResult,
		StartPoint,
		EndPoint,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2,
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	if (bResult && HasAuthority()) //클라이언트에서 또 충돌체크 방지!
	{
		UE_LOG(LogTemp, Log, TEXT("ABossCharacter::AttackCheck - Something Hit"));
		AActor* HitActor = HitResult.GetActor(); //부딪힌 액터의 정보를 가져온다.
		if (IsValid(HitActor))
		{
			UE_LOG(LogTemp, Log, TEXT("Hit Actor Name : %s"), *HitActor->GetName());
		}

		if (HitActor && HitActor->ActorHasTag(FName("Player")))
		{
			ASoulCharacter* Player = Cast<ASoulCharacter>(HitActor);
			if (Player)
			{
				

				Player->AttachToComponent(
					GetMesh(),
					FAttachmentTransformRules::SnapToTargetNotIncludingScale,
					//FName("Grab")
					FName("rootGrab")
				);

				Player->OnGrabed();
				

				UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
				if (AnimInstance)
				{
					AnimInstance->Montage_JumpToSection(FName("OnGrab"));
				}

				UMyBossAnimInstance* MyBossAnimInstance = Cast<UMyBossAnimInstance>(AnimInstance);
				MyBossAnimInstance->SetOnIk(true);

			}

			FTimerHandle TimerHandle;

			GetWorldTimerManager().SetTimer(TimerHandle, [this, Player]() //람다 : []안에는 사용할 변수들을 꼭 넣어야한다.
				{
					if (Player)
					{
						Player->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
						UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
						UMyBossAnimInstance* MyBossAnimInstance = Cast<UMyBossAnimInstance>(AnimInstance);
						if (MyBossAnimInstance)
						{
							MyBossAnimInstance->SetOnIk(false);
						}
					}
				}, 3.0f, false);
		}

	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("ABossCharacter::AttackCheck - No Hit"));
	}
}
