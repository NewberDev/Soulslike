// Fill out your copyright notice in the Description page of Project Settings.


#include "MyCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputSubsystems.h" 
#include "EnhancedInputComponent.h"
#include "MyAnimInstance.h"
#include "DrawDebugHelpers.h" //12/1 - 디버깅 드로잉을 위한 헤더
#include "Kismet/GameplayStatics.h" //12/3 - 데이지 주기 & 게임모드 가져오기
#include "Net/UnrealNetwork.h" //Replication 구현을 위한 헤더
#include "MyGameModeBase.h"
#include "Bullet.h"
#include "Camera/CameraComponent.h" //카메라 컴포넌트
#include "GameFramework/SpringArmComponent.h" //스프링암 컴포넌트
#include "ComboAttackAttributeSet.h"
#include "CharacterStatAttributeSet.h"


// Sets default values
AMyCharacter::AMyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	
	/* //BP내에서 대신한다.
	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();


	//CreateDefaultSubobject와 ConstructorHelpers::FObjectFinder의 용도를 잘 구별하자!
	Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	Arms->SetupAttachment(CapsuleComp); //핵심
	

	ConstructorHelpers::FObjectFinder<USkeletalMesh> ArmsMesh(
		TEXT("/Game/FirstPersonArms/Character/Mesh/SK_Mannequin_Arms.SK_Mannequin_Arms"));
	
	if (ArmsMesh.Succeeded())
	{
		Arms->SetSkeletalMesh(ArmsMesh.Object);
	}
	*/


	//11/26 - 콤보 공격
	MaxCombo = 4;
	IsComboInputOn = false; //시작시 콤보 false
	CanNextCombo = false; //시작시 다음 콤보 false
	CurrentCombo = 0;

	//12/3 - 디버깅 드로잉
	AttackRange = 120.f;
	AttackRadius = 50.f;

	CurrentHp = 10.f;
	MaxHp = 100.f; 

	SeverRPC_IsAttackingCheck(false);

	/* //코드에서 카메라 만들기
	// SpringArm
    CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
    CameraBoom->SetupAttachment(RootComponent);
    CameraBoom->TargetArmLength = 300.f;
    CameraBoom->bUsePawnControlRotation = true;

    // Camera
    FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
    FollowCamera->SetupAttachment(CameraBoom);
    FollowCamera->bUsePawnControlRotation = false;
	*/

	//어빌리티시스템 컴포넌트 추가-----------------
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true); //멀티플레이를 고려한 리플리케이션

	ComboAttackAttributeSet = CreateDefaultSubobject<UComboAttackAttributeSet>(TEXT("ComboAttackAttributeSet"));

	CharacterStatAttributeSet = CreateDefaultSubobject<UCharacterStatAttributeSet>(TEXT("CharacterStatAttributeSet"));
	//---------------------------------------------

}

// Called when the game starts or when spawned
void AMyCharacter::BeginPlay()
{
	Super::BeginPlay();


	//12/24 - 서버와 클라이언트 모두 적용되는 컨트롤러 적용만들기
	ReceiveControllerChangedDelegate.AddDynamic(this, &AMyCharacter::OnControllerChanged);
	//위 함수가 너무 일찍실행돼서 오류가 날수 있어서 아래에 한번더 실행해본다.
	AController* BeginController = GetController();
	if (BeginController)
	{
		OnControllerChanged(this, nullptr, BeginController);
	}
	
	//게임 모드를 가져와서 델리게이트 등록하기
	/*
	AMyGameModeBase* MyGameMode = Cast<AMyGameModeBase>(UGameplayStatics::GetGameMode(GetWorld())); //현재 게임의 모드를 가져오는것이다.
	if (MyGameMode)
	{
		MyGameMode->OnCharacterSpawned(this); 
		//CurrentHP가 0이 될때 실행될 델리게이트를 게임모드에서 등록되기에
		//등록하기 위한 게임모드의 함수를 플레이어의 클래스에서 가져와 실행한다.
		
	}
	*/ //이 방법은 게임모드 베이스가 이때 없어서 실행이 안되는 문제가 발생할수 있다.


	WeaponGeneration(); //처음 플레이어가 가진 손& 허리에 생성 

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		FGameplayEffectContextHandle Content = AbilitySystemComponent->MakeEffectContext();
		//게임 플레이 이펙트 변수로 생성

		if (DefaultAttributeEffect)
		{
			Content.AddSourceObject(this);
			AbilitySystemComponent->ApplyGameplayEffectToSelf(
				DefaultAttributeEffect->GetDefaultObject<UGameplayEffect>(),
				1,
				Content
			); //이펙트를 등록하는 함수이다.
		}
		 

		if (HasAuthority()) //서버의 권한을 갖는지 체크
		{ //권한이 있을때 
			if (AttackAbilities)
			{
				AttackAbilityHander = AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AttackAbilities));

				
			}

		}
		//if(GetWorld()->IsNetMode(EnetMode::NM_ListenServer));
		//서버인지 확인하는 다른 방법!
	}
}

//11/24 -> 즉 애니메이션이 진행되고 끝나기 전에 다음 콤보를 이어갈것인지를 결정하도록 하는것!
//11/26 추가
void AMyCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());

	if (AnimInstance) //성공적으로 받아왔을때,
	{
		AnimInstance->OnMontageEnded.AddDynamic(this, &AMyCharacter::OnAttackMontageEnded);

		//다음 콤보로 갈지 체크할때 동작하는 함수!
		AnimInstance->OnNextAttackCheckDelegate.AddLambda([this]()-> void {
			CanNextCombo = false;
			if (IsComboInputOn)
			{
				AttackStartComboState();
				AnimInstance->JumpToAttackMontageSection(CurrentCombo);
				//다음 콤보 애니메이션으로 넘어가라는 동작
			}
		});
		/*
		* 람다식: 필기 참고 
		*/
		//12/1 - 충돌구현
		AnimInstance->OnAttackHitCheckDelegate.AddUObject(this, &AMyCharacter::AttackCheck); 
		//AttackCheck함수를 델리게이트로 등록한다.

		
		AnimInstance->OnBulletFireDelegate.AddUObject(this, &AMyCharacter::ServerRPC_BulletGeneration);


		AnimInstance->OnOnWeaponDelegate.AddUObject(this, &AMyCharacter::OnWeapon);
		AnimInstance->OnOffWeaponDelegate.AddUObject(this, &AMyCharacter::OffWeapon);

	}
}

// Called every frame
void AMyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AMyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);


	//헤더 추가 필요 "EnhancedInputComponent.h"
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhancedInputComponent) //형변환 잘했나 확인
	{
		if (MoveAction)//MoveAction이 잘 지정되어 있는지 확인
			EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMyCharacter::Move);
		//Triggered외에, Ongoing, Started 등이 있는데
		//Triggered는 누루는 동안 계속 이동하는 방식을 말함

	}

	if (LookAction) //BP Editer에서 액션을 적절히 지정하지 않았을 경우에도 문제가 없도록 하는 것이다.
	{
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMyCharacter::Look);
							 //BindAction: 함수와 Input(컴포넌트)를 연결하는 코드
	}

	if (JumpAction) //BP Editer에서 액션을 적절히 지정하지 않았을 경우에도 문제가 없도록 하는 것이다.
	{ //스페이스 - 점프 만들기
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &AMyCharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &AMyCharacter::StopJumping);

		//점프는 누르는 동안이 아니라, 누른 순간이기에 Started이다
		//점프의 끝은 동작의 끝이기에, Completed이다
		//Jump()와 StopJumping() 언리얼에서 기본 제공 함수이다.
	}
	if (AttackAction) //연결이 잘되어 있으면 실행
	{
		EnhancedInputComponent->BindAction(AttackAction, ETriggerEvent::Started, this, &AMyCharacter::Attack);
		//AttackAction 키 작동 시 - > Attack 함수 실행
		//위처럼 BindAction으로 엮은 함수들을 바인딩했다고 한다.
	}
	if (RollAction) 
	{
		//UE_LOG(LogTemp, Warning, TEXT("AMyCharacter::SetupPlayerInputComponent_buttonAction"));
		EnhancedInputComponent->BindAction(RollAction, ETriggerEvent::Started, this, &AMyCharacter::Roll);
		
	}
	if (FireAction)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AMyCharacter::SetupPlayerInputComponent_buttonAction"));
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AMyCharacter::Fire);

	}
	if (WeaponAction)
	{
		//UE_LOG(LogTemp, Warning, TEXT("AMyCharacter::SetupPlayerInputComponent_WeaponAction"));
		EnhancedInputComponent->BindAction(WeaponAction, ETriggerEvent::Started, this, &AMyCharacter::WeaponCheck);

	}
}


//(2) 이동구현을 위한 코드로 이게 찐으로 중요!!!! - 공부하고 외우자!!!!
void AMyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	//위의 코드는 부모클래스에서 어떤 내용이 담겼을지 모르기에
	//그 부모 클래스의 현 가상함수도 같이 실행해주는 코드이다.
	//참고로 Super은 부모클래스 타입을 말한다.

	/*
	//플레이어로 형변환
	APlayerController* PC = Cast<APlayerController>(NewController);
	if (PC) //현변황이 잘되었나 확인
	{
		ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
		//예를 들어 멀티플레이일때, 컨트롤이 여러개중에 내꺼를 가져와야하기에
		//LocalPlayer로 가져오는 것이다.
		if (LocalPlayer) //잘 가져왔는지 확인
		{

			//Subsystem이 무엇인지는 나중에 더 배울것이다. - 헤더 추가 필요 "EnhancedInputSubsystems.h"
			UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

			if (InputSubsystem) //잘 가져왔나 확인 - 언리얼은 안전 추구가 중요하기에 항상 가져오면 잘 가져왔는지 확인해야한다.
			{
				InputSubsystem->AddMappingContext(InputMappingContext, 0);
				//입력매핑 컨텍스트와 우선순위라는 두가지 파라미터를 받는다.
				
				  //플레이어 하나에 여러개의 컨트롤러가 지정될수가 있다.
				  //예를 들어 걷다, 차타다, 비행기 타다 할때 여러개가 될때
				  //지웠다 다시하기 보다 저렇게 우선순위를 둬서 Controller를
				  //결정하도록 하는 역할이다.
				  //숫자가 높아야지 우선순위가 높은 것이다.!!!!
				
			}
		}

	}
	*/ //밑에 함수로 대체한다. 
	//Possessedby함수는 서버에서만 작동, 
	//아래의 OnControllerChanged는 서버, 클라이언트 모두 작동한다.
}

//12/24 - 서버와 클라이언트 모두 적용되는 컨트롤러 적용만들기
void AMyCharacter::OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	//플레이어로 형변환
	APlayerController* PC = Cast<APlayerController>(NewController);
	if (PC) //현변황이 잘되었나 확인
	{
		ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();
		/*예를 들어 멀티플레이일때, 컨트롤이 여러개중에 내꺼를 가져와야하기에
		LocalPlayer로 가져오는 것이다.*/
		if (LocalPlayer) //잘 가져왔는지 확인
		{

			//Subsystem이 무엇인지는 나중에 더 배울것이다. - 헤더 추가 필요 "EnhancedInputSubsystems.h"
			UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

			if (InputSubsystem) //잘 가져왔나 확인 - 언리얼은 안전 추구가 중요하기에 항상 가져오면 잘 가져왔는지 확인해야한다.
			{
				InputSubsystem->AddMappingContext(InputMappingContext, 0);
				//입력매핑 컨텍스트와 우선순위라는 두가지 파라미터를 받는다.
				/*
				  플레이어 하나에 여러개의 컨트롤러가 지정될수가 있다.
				  예를 들어 걷다, 차타다, 비행기 타다 할때 여러개가 될때
				  지웠다 다시하기 보다 저렇게 우선순위를 둬서 Controller를
				  결정하도록 하는 역할이다.
				  숫자가 높아야지 우선순위가 높은 것이다.!!!!
				*/
			}
		}

	}
}

void AMyCharacter::Move(const FInputActionValue& Value)
{
	//입력에 따라 화면에 출력되도록 하는 함수
	
	/*
	FVector2D Input1 = Value.Get<FVector2D>();
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%f / %f"), Input1.X, Input1.Y));
	*/

	//이 함수의 파라미터(메세지 고유키값(-1/0), 메세지 출력 시간, 색결정, 문자열)
	//FString::Printf은 우리에게 익숙한 출력문인데, TEXT()메크로로 묶어서넣어야한다.


	/*
	* AddMovemetnInput - AddTransform을 쓰지 않고 이것을 쓰는 이유
	*       이전의 배운 방식에서는 Tick에서 deltaTime으로 속도 조정이 필요했지만,
	*		현재 방식은 Tick에 다시 반복할 필요가 없으며,
	*       "액터 이동에 따른 위치 동기화"개념의 이득을 보는데
	*		예측이동, 위치 보간, 정보 압축 전달의 이점이 있다.
	*		멀티 시에 다른 pc에서는 내 액터 이동을 알수 없기에
	*		이 정보를 효율적으로 주고 받고록 설계된 함수 이다.
	*/
	FVector2D Input = Value.Get<FVector2D>();
	FVector MoveDirection = GetActorForwardVector() * Input.Y + GetActorRightVector() * Input.X;
	//위에 것만 하면 대각선 입력시에 X와 Y가 1이라면 대각선은 루트2만큼 크게 들어간다.
	MoveDirection.Normalize(); //대각선도 단위벡터로 정규화하는 코드!!!
	AddMovementInput(MoveDirection);
	//11/24
	//IsAttacking = true;

	//Jump(); //캐릭터가 점프를 하는 언리얼 제공 함수

}

void AMyCharacter::Look(const FInputActionValue& Value)
{
	FVector2D Input = Value.Get<FVector2D>();
	AddControllerPitchInput(Input.Y);
	AddControllerYawInput(Input.X);
}


/* //직접 점프 변수 만들기
void AMyCharacter::MyJump()
{
	Jump();
	UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	MyAnimInstance->isJumpStart = true;
}
*/

void AMyCharacter::Attack() 
{
	//if (!UsingWeapon) return;
	//IA_Attack > IMC > C++코드가 받아서 마우스 왼쪽 누를시에 현재 함수가 실행된다.
	//MyAnimInstance에서 애니메이션 몽타주를 받아온다.
	
	/*
	//UMyAnimInstance* MyAnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	if (!AnimInstance) return; //유효하지 않으면 넘기기

	//11/26 콤보 어택 구성
	if (IsAttacking)
	{
		if (CanNextCombo)
		{
			IsComboInputOn = true;
		}
	}
	else //첫 공격 실행이라면
	{
		AttackStartComboState();
		AnimInstance->PlayAttackMontage();//애니메이션 몽타주 실행.
		AnimInstance->JumpToAttackMontageSection(CurrentCombo);
		//IsAttacking = true;
		SeverRPC_IsAttackingCheck(true);

		
		SeverRPC_PlayAttackMontage(CurrentCombo);
		//서버 RPC는 이 경우 서버에서도 그냥 실행하기에 두번 실행하는 문제가 날수 있다

		
	}
	*/ //어빌리티 사용을 위해 잠시 공격 제한


	UE_LOG(LogTemp, Log, TEXT("Attack"));

	//GiveAbility로 등록한 어빌리티를 실행하는 코드!
	AbilitySystemComponent->TryActivateAbility(AttackAbilityHander);
	
}

//11/24 : 콤보가 끝났을떄 실행되는 함수 - 언리얼 기본 실행
void AMyCharacter::OnAttackMontageEnded(UAnimMontage* Montage, bool bInterrupted)
{
	//콤보가 끝나는 부분에서 아래의 TEXT가 호출된다.
	//UE_LOG(LogTemp, Log, TEXT("AMyCharacter::OnAttackMontageEnded"));

	//IsAttacking = false;
	SeverRPC_IsAttackingCheck(false);
	AttackEndComboState();

}



//11/26 - 콤보 어택
void AMyCharacter::AttackStartComboState() //콤보 시작시 호출(사용자화)
{
	CanNextCombo = true;
	IsComboInputOn = false;
	CurrentCombo = FMath::Clamp<int32>(CurrentCombo + 1, 1, MaxCombo);
}
void AMyCharacter::AttackEndComboState() //콤보 종료시 실행될 함수(사용자화)
{
	IsComboInputOn = false;
	CanNextCombo = false;
	CurrentCombo = 0;
}


//12/1 - 충돌구현
void AMyCharacter::AttackCheck() 
{
	//UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck")); //함수 실행 확인
	FHitResult HitResult;
	FCollisionQueryParams Params(NAME_None, false, this);
	bool bResult = GetWorld()->SweepSingleByChannel( //레이저 쏘는 함수
		HitResult,
		GetActorLocation(),
		GetActorLocation() + GetActorForwardVector() * AttackRange,
		FQuat::Identity,
		ECollisionChannel::ECC_GameTraceChannel2, //임시 수정중!*********.
		FCollisionShape::MakeSphere(AttackRadius),
		Params
	);

	if (bResult)
	{
		UE_LOG(LogTemp, Log, TEXT("AMyCharacter::AttackCheck - Something Hit"));
		AActor* HitActor = HitResult.GetActor(); //부딪힌 액터의 정보를 가져온다.
		if (IsValid(HitActor))
		{
			UE_LOG(LogTemp, Log, TEXT("Hit Actor Name : %s"), *HitActor->GetName());
		}
		//12/3 - 대미지 주기 - 아래의 함수가 실행되면 그값이 TakeDamage의 파라미터인 DamageAmount 값으로 전달된다.
		/*
		UGameplayStatics::ApplyDamage(
			HitActor,
			1.0f,
			GetInstigatorController(),
			this,
			UDamageType::StaticClass()
		);*/ //서버RPC로 이동

		
		ServerRPC_ApplyDamage(HitActor);
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


//즉 ApplyDamage가 실행되면 TakeDamage가 연계된다.
/*
	TakeDamage 함수는 항상 서버에서만 실행된다.
	즉, 공격한 쪽이 서버라면 ApplyDamage로 인해 TakeDamage가 실행되지만,
	반대로 클라이언트에서는 공격해서 ApplyDamage가 실행돼도 TakeDamage가 실행되지 않는다.
*/
float AMyCharacter::TakeDamage(
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

	//AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());
	if (!AnimInstance->IsRollMontagePlaying())
	{
		CurrentHp -= Damage;
	}

	

	UE_LOG(LogTemp, Log, TEXT("CurrentHealth: %f"), CurrentHp);
	
	//상대 액터에 대미지 주기!
	/*
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f, FColor::Green,
		FString::Printf(TEXT("CurrentHP: %f"), CurrentHp)
	); */

	ClientRPC_ShowDamage(CurrentHp);

	if (CurrentHp <= 0)
	{
		//래그돌로 바꾸기
		DoRagdoll(); //TakeDamage가 서버에서 실행되니게 이 함수는 서버RPC가 아니어도 된다.
	
		//죽었을때 실행되는 데리게이트를 호출한다. - 게임 규칙(게임 모드)에게 정보 전달
		/*
		* 게임 오버, UI 띄우기 등은 플레이어 클래스가 아니라 
		* 게임 규칙에서 담당하도록 게임 규칙에 정보를 전달하는 것이다.
		*/

		OnMyCharacterDead.Broadcast(this);
	}
	return Damage;
}


void AMyCharacter::OnRep_CurrentHP()
{

}

//Replication 구현하기
void AMyCharacter::GetLifetimeReplicatedProps(
	TArray<FLifetimeProperty>& OutLifetimeProps
) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AMyCharacter, CurrentHp); //이 표현으로 결국 이 변수는 실행동안 리플리케이션 된다.
	//DOREPLIFETIME(AMyCharacter, IsAttacking);
}


void AMyCharacter::Roll()
{
	SeverRPC_PlayRollMontage();
}

void AMyCharacter::Fire()
{
	if (UsingWeapon) return;
	//구체발사 -서버RPC 생성만 하면 레플리케이션으로 클라이언트에 공유될듯?
	ServerRPC_Fire();
}

void AMyCharacter::WeaponGeneration()
{
	UE_LOG(LogTemp, Log, TEXT("AMyCharacter::WeaponGeneration"));

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetActorLocation());

	FRotator SpawnRotation = FRotator::ZeroRotator;

	//SpawnTransform.SetRotation(GetActorForwardVector().Rotation.Quaternion);
	if (Controller)
	{
		SpawnTransform.SetRotation(Controller->GetControlRotation().Quaternion());
	}
	SpawnTransform.SetScale3D(FVector(1.f));


	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	savesword = GetWorld()->SpawnActor<AActor>(
		SavedWeapon,
		SpawnTransform,
		SpawnParams
	); //-> SetOwer(this);

	usesword = GetWorld()->SpawnActor<AActor>(
		UseWeapon,
		SpawnTransform,
		SpawnParams
	); //-> SetOwer(this);

	if (!savesword)
	{
		UE_LOG(LogTemp, Log, TEXT("savesword-Invalid"));
		return;
	}
	if (!usesword)
	{
		UE_LOG(LogTemp, Log, TEXT("savesword-Invalid"));
		return;
	}

	CharacterMesh = GetMesh();

	savesword->AttachToComponent(
		CharacterMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		"WeaponSave"
	);

	usesword->AttachToComponent(
		CharacterMesh,
		FAttachmentTransformRules::SnapToTargetNotIncludingScale,
		"RWeapon"
	);

	usesword->SetActorHiddenInGame(true);
	UsingWeapon = false;
}

void AMyCharacter::WeaponCheck()
{
	if (!(AnimInstance->GetCurrentActiveMontage()))
	{
		if (UsingWeapon)
		{
			AnimInstance->PlayOffWeaponMontage();
		}
		else
		{
			AnimInstance->PlayOnWeaponMontage();
		}
	}

	
}

void AMyCharacter::OnWeapon()
{
	UsingWeapon = true;
	usesword->SetActorHiddenInGame(false);
	savesword->SetActorHiddenInGame(true);

}

void AMyCharacter::OffWeapon()
{

	UsingWeapon = false;
	usesword->SetActorHiddenInGame(true);
	savesword->SetActorHiddenInGame(false);
}

void AMyCharacter::SeverRPC_PlayRollMontage_Implementation()
{
	MultiCastRPC_PlayRollMontage();
}

void AMyCharacter::MultiCastRPC_PlayRollMontage_Implementation()
{
	AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());

	AnimInstance->PlayRollMontage();//애니메이션 몽타주 실행.
}

//12/29 - 상대 대미지 주기!
void AMyCharacter::ServerRPC_ApplyDamage_Implementation(AActor* HitActor)
{
	UGameplayStatics::ApplyDamage(
			HitActor, //딜 대상
			1.0f, // 딜 수치
			GetInstigatorController(),
			this, //딜 주는 대상
			UDamageType::StaticClass() //딜의 종류
		);
}

void AMyCharacter::ClientRPC_ShowDamage_Implementation(float Hp)
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f, FColor::Green,
		FString::Printf(TEXT("CurrentHP: %f"), Hp)
	);
}

void AMyCharacter::SeverRPC_PlayAttackMontage_Implementation(int32 CurCombo)
{
	MultiCastRPC_PlayAttackMontage(CurCombo);
	
}

void AMyCharacter::MultiCastRPC_PlayAttackMontage_Implementation(int32 CurCombo)
{
	/* 로컬 플레이어는 패스
		즉, 멀티캐스트는 서버에서 또 한번 실행하기에 클라이언트에서 2번 실행되는 문제가 발생한다.
		이를 막기 위해서 아래에 같은 컨트롤러를 쓰는 즉, 실행된 클라이언트에서는 한번만 실행하도록 제한하는 코드이다.
	*/
	if (!IsLocallyControlled())  
	{
		AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());

		AnimInstance->PlayAttackMontage();//애니메이션 몽타주 실행.
		AnimInstance->JumpToAttackMontageSection(CurCombo);

	}
}

void AMyCharacter::DoRagdoll()
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	SkeletalMesh->SetSimulatePhysics(true);
	Mytransform = SkeletalMesh->GetRelativeTransform();

}

void AMyCharacter::Restrart()
{
	USkeletalMeshComponent* SkeletalMesh = GetMesh();
	SkeletalMesh->SetRelativeTransform(Mytransform);
}


void AMyCharacter::SeverRPC_IsAttackingCheck_Implementation(bool TF)
{
	IsAttacking = TF;
}


void AMyCharacter::ServerRPC_Fire_Implementation()
{
	GEngine->AddOnScreenDebugMessage(
		-1,
		5.f, FColor::Blue,
		FString::Printf(TEXT("Spawn Actor"))
	);
	
	
	
	//몽타주 재생-> 멀티 캐스트
	MultiCastRPC_PlayFireMontage();
}

void AMyCharacter::ServerRPC_BulletGeneration_Implementation()
{

	UE_LOG(LogTemp, Log, TEXT("AMyCharacter::ServerRPC_BulletGeneration"));

	//액터 생성
	//FTransform transform;
	//transform.SetLocation(FVector(1,2,3));
	//transform.SetRotation(FQuat(FRotator(30,20,50)));


	//FTransform transform = FTransform(FVector(900, 950, 100)); //회전 0,0,0 값으로 적용
	
	//FTransform transform;
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	FTransform SpawnTransform;
	SpawnTransform.SetLocation(GetActorLocation() + GetActorForwardVector()* 100.f);

	FRotator SpawnRotation = FRotator::ZeroRotator;

	//SpawnTransform.SetRotation(GetActorForwardVector().Rotation.Quaternion);
	if (Controller)
	{
		SpawnTransform.SetRotation(Controller->GetControlRotation().Quaternion());
	}
	SpawnTransform.SetScale3D(FVector(1.f));



	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ABullet* bullet = GetWorld()->SpawnActor<ABullet>(
		SpawnActorClass,
		SpawnTransform,
		SpawnParams
	); //-> SetOwer(this);

	if (!bullet)
	{
		UE_LOG(LogTemp, Log, TEXT("Bullet-Invalid"));
	}
}

void AMyCharacter::MultiCastRPC_PlayFireMontage_Implementation()
{
	AnimInstance = Cast<UMyAnimInstance>(GetMesh()->GetAnimInstance());

	AnimInstance->PlayFireMontage();//애니메이션 몽타주 실행.
}


void AMyCharacter::EnemyFind_CameraAction(AActor* Enemy)
{

}


//어빌리티 이용에 -> 인터페이스 사용을 위해 오버라이딩 하는 함수
UAbilitySystemComponent* AMyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}