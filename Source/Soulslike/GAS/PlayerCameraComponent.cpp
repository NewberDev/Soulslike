// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"

// Sets default values for this component's properties
UPlayerCameraComponent::UPlayerCameraComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UPlayerCameraComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCameraComponent: Owner is null."));
		return;
	}

	if (!CachedSpringArm)
	{
		CachedSpringArm = OwnerActor->FindComponentByClass<USpringArmComponent>();
	}
	if (!CachedSpringArm)
	{
		UE_LOG(LogTemp, Warning, TEXT("PlayerCameraComponent: SpringArm not found on %s"), *OwnerActor->GetName());
		return;
	}

	OriginalLength = CachedSpringArm->TargetArmLength;

	
}


// Called every frame
void UPlayerCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
	if (bIsZooming)
	{
		ZoomEnemy(DeltaTime);
	}

	if(bIsDeadMoving)
	{
		PlayerDeadMoving(DeltaTime);
	}
}

void UPlayerCameraComponent::ZoomEnemy(float DeltaTime)
{
	if (!Enemy || !CachedSpringArm)
	{
		bIsZooming = false;
		UnlockOwnerPlayerInput();
		return;
	}

	CameraTimer += DeltaTime;

	const float ZoomAlpha = FMath::Clamp(CameraTimer / ZoomEnemyDuration, 0.f, 1.f);

	switch (ZoomEnemyPattern)
	{
	case 0:
		// 선형: 줌 시작 시점 OriginalLength -> ZoomEnemyLength
		CachedSpringArm->TargetArmLength = FMath::Lerp(OriginalLength, ZoomEnemyLength, ZoomAlpha);
	

		if (ZoomAlpha >= 1.f - KINDA_SMALL_NUMBER)
		{
			CachedSpringArm->TargetArmLength = ZoomEnemyLength;
			ZoomEnemyPattern = 1;
			CameraTimer = 0.f;
			UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::ZoomEnemy - Pattern 0 Clear"));
		}
		break;
	case 1:
		if (CameraTimer >= 2.2f)
		{
			ZoomEnemyPattern = 2;
			CameraTimer = 0.f;
			UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::ZoomEnemy - Pattern 1 Clear"));

		}
		break;
	case 2:
		CachedSpringArm->TargetArmLength = FMath::Lerp(ZoomEnemyLength, OriginalLength, ZoomAlpha);

		if (ZoomAlpha >= 1.f - KINDA_SMALL_NUMBER)
		{
			CachedSpringArm->TargetArmLength = OriginalLength;
			ZoomEnemyPattern = 0;
			CameraTimer = 0.f;
			bIsZooming = false;
			UnlockOwnerPlayerInput();
			UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::ZoomEnemy - Pattern 2 Clear"));

		}
		break;
	}
}

void UPlayerCameraComponent::SetEnemy(AActor* NewEnemy)
{

	UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::SetEnemy"));
	Enemy = NewEnemy;
}

void UPlayerCameraComponent::ZoomEnemyTrigger()
{
	

	if (!Enemy)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::ZoomEnemyTrigger: Enemy is null"));
		return;
	}
	if(!CachedSpringArm)
	{
		UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::ZoomEnemyTrigger: CachedSpringArm is null"));
		return;
	}

	const FVector EnemyLocation = Enemy->GetActorLocation();
	const FVector PlayerLocation = GetOwner()->GetActorLocation();

	// 거리에 따른 목표 암 길이 (필요 시 최소/최대로 클램프)
	ZoomEnemyLength = 800.f - FVector::Distance(EnemyLocation, PlayerLocation);

	// 줌 시작 시점의 길이를 기준으로 복귀
	OriginalLength = CachedSpringArm->TargetArmLength;


	LookAtEnemy();


	LockOwnerPlayerInput();



	bIsZooming = true;
	ZoomEnemyPattern = 0;
	CameraTimer = 0.f;
}

void UPlayerCameraComponent::LookAtEnemy()
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::LookAtEnemy"));

	APawn* OwnerPawn = Cast<APawn>(GetOwner());

	if (!OwnerPawn || !Enemy)
	{
		return;
	}

	//멀티시 고려
	if (!OwnerPawn->IsLocallyControlled())
	{
		return;
	}

	if (AController* Controller = OwnerPawn->GetController())
	{
		const FVector Start = OwnerPawn->GetActorLocation();
		FVector Target = Enemy->GetActorLocation();
		Target.Z += 50.f; // 줌의 초점을 Z축으로 올린다.

		FVector Dir = Target - Start;
		//Dir.Z = 0.f;
		if (!Dir.Normalize()) return;

		const FRotator TargetRot = Dir.Rotation();
		Controller->SetControlRotation(FRotator(TargetRot.Pitch, TargetRot.Yaw, 0.f));
	}

	UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::LookAtEnemy"));
}

void UPlayerCameraComponent::LockOwnerPlayerInput()
{
	UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::LockOwnerPlayerInput"));
	APawn* Pawn = Cast<APawn>(GetOwner());
	//멀티시 고려
	if (!Pawn || !Pawn->IsLocallyControlled())
	{
		return;
	}
	
	if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::LockOwnerPlayerInput - DisableInput"));

		Pawn->DisableInput(PC);
	}
}

void UPlayerCameraComponent::UnlockOwnerPlayerInput()
{

	APawn* Pawn = Cast<APawn>(GetOwner());
	
	if (!Pawn || !Pawn->IsLocallyControlled())
	{
		return;
	}
	if (APlayerController* PC = Cast<APlayerController>(Pawn->GetController()))
	{
		UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::LockOwnerPlayerInput - EnableInput"));
		Pawn->EnableInput(PC);
	}
}

void UPlayerCameraComponent::CameraShaking_Scene()
{
	// 1. 이 컴포넌트를 들고 있는 액터를 폰(Pawn)으로 캐스팅합니다.
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	// 2. 폰으로부터 플레이어 컨트롤러를 가져옵니다.
	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC) return;

	// 3. 에디터에서 쉐이크 클래스 에셋을 등록했는지 확인하고 실행합니다.
	if (SceneCameraShakeClass)
	{
		// 첫 번째 인자: 실행할 쉐이크 클래스
		// 두 번째 인자: 스케일(배율) 수치 (1.0f가 기본, 숫자가 커질수록 더 세게 흔들림)
		PC->ClientStartCameraShake(SceneCameraShakeClass, 1.0f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::CameraShaking - SceneCameraShakeClass가 비어있습니다! 디테일 패널에서 에셋을 등록하세요."));
	}
}

void UPlayerCameraComponent::CameraShaking_Earthquake()
{
	// 1. 이 컴포넌트를 들고 있는 액터를 폰(Pawn)으로 캐스팅합니다.
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	// 2. 폰으로부터 플레이어 컨트롤러를 가져옵니다.
	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC) return;

	// 3. 에디터에서 쉐이크 클래스 에셋을 등록했는지 확인하고 실행합니다.
	if (EarthquakeCameraShakeClass)
	{
		// 첫 번째 인자: 실행할 쉐이크 클래스
		// 두 번째 인자: 스케일(배율) 수치 (1.0f가 기본, 숫자가 커질수록 더 세게 흔들림)
		PC->ClientStartCameraShake(EarthquakeCameraShakeClass, 1.0f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::CameraShaking - SceneCameraShakeClass가 비어있습니다! 디테일 패널에서 에셋을 등록하세요."));
	}
}

void UPlayerCameraComponent::CameraShaking_Damage()
{
	// 1. 이 컴포넌트를 들고 있는 액터를 폰(Pawn)으로 캐스팅합니다.
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn) return;

	// 2. 폰으로부터 플레이어 컨트롤러를 가져옵니다.
	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC) return;

	// 3. 에디터에서 쉐이크 클래스 에셋을 등록했는지 확인하고 실행합니다.
	if (DamageCameraShakeClass)
	{
		// 첫 번째 인자: 실행할 쉐이크 클래스
		// 두 번째 인자: 스케일(배율) 수치 (1.0f가 기본, 숫자가 커질수록 더 세게 흔들림)
		PC->ClientStartCameraShake(DamageCameraShakeClass, 1.0f);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("UPlayerCameraComponent::CameraShaking - DamageCameraShakeClass가 비어있습니다! 디테일 패널에서 에셋을 등록하세요."));
	}
}

void UPlayerCameraComponent::PlayerDeadMovingTrigger()
{
	bIsDeadMoving = true;

	CameraTimer = 0;
}

void UPlayerCameraComponent::PlayerDeadMoving(float DeltaTime)
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (!Pawn || !Pawn->IsLocallyControlled()) { bIsDeadMoving = false; return; }

	APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
	if (!PC) { bIsDeadMoving = false; return; }

	// 현재 회전
	const FRotator CurrentRot = PC->GetControlRotation();

	// 아래 방향을 바라보는 목표 회전

	//const FRotator TargetRot = FRotationMatrix::MakeFromX(FVector(0.f, 0.f, -1.f)).Rotator();

	// 1. 누워있는 메시의 Yaw 값을 가져옴
	float MeshYaw = Pawn->GetComponentByClass<USkeletalMeshComponent>()->GetComponentRotation().Yaw;

	// 2. 메시 축이 왼쪽으로 돌아가 있다면 90도를 더해줌 (안 맞으면 - 90.f로 변경)
	MeshYaw += 90.f;

	// 3. 보정된 Yaw 값을 타겟으로 설정
	const FRotator TargetRot = FRotator(-89.9f, MeshYaw, 0.f);

	// 부드럽게 회전
	const float InterpSpeed = 0.5f;

	const FRotator NewRot =
		FMath::RInterpTo(CurrentRot, TargetRot, DeltaTime, InterpSpeed);

	PC->SetControlRotation(NewRot);

	// 거의 도착했는지 체크
	if (NewRot.Equals(TargetRot, 1.f))
	{
		PC->SetControlRotation(TargetRot);
		bIsDeadMoving = false;
	}
}