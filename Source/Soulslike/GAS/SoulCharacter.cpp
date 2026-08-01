// Fill out your copyright notice in the Description page of Project Settings.


#include "SoulCharacter.h"
#include "EnhancedInputSubsystems.h" //UEnhancedInputLocalPlayerSubsystem?? ????
#include "EnhancedInputComponent.h"
#include "SoulGameplayTags.h"
#include "SoulAnimInstance.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"


// Sets default values
ASoulCharacter::ASoulCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;


	//컴포넌트 셋팅
	AbilitySystemComponent = CreateDefaultSubobject<USoulAbilitySystemComponent>(TEXT("SoulAbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);

	CharacterComponent = CreateDefaultSubobject<USoulCharacterComponent>(TEXT("SoulCharacterComponent"));
	CombatComponent = CreateDefaultSubobject<USoulCombatComponent>(TEXT("SoulCombatComponent"));


	SoulAttributeSet = CreateDefaultSubobject<USoulAttributeSet>(TEXT("SoulAttributeSet"));

	PlayerCameraComponent = CreateDefaultSubobject<UPlayerCameraComponent>(TEXT("PlayerCameraComponent"));
}

// Called when the game starts or when spawned
void ASoulCharacter::BeginPlay()
{
	Super::BeginPlay();

	//IMC
	ReceiveControllerChangedDelegate.AddDynamic(this, &ASoulCharacter::OnControllerChanged);
	AController* BeginController = GetController();
	if (BeginController)
	{
		OnControllerChanged(this, nullptr, BeginController);
	}

	


	CharacterMesh = GetMesh();

	WeaponGeneration();

	//SoulAttributeSet->InitMaxSP(100.f);
	//SoulAttributeSet->InitSP(100.f);
}

void ASoulCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this); //ASC ????

		GiveAbilitySet(); 



		CombatComponent->GiveGA_Death();

		AActor* This = Cast<AActor>(this);

		UE_LOG(LogTemp, Warning, TEXT("%s : Ability Num: %d"), *This->GetName(), AbilitySystemComponent->GetActivatableAbilities().Num());

		USoulAttributeSet* ActiveAttributeSet = const_cast<USoulAttributeSet*>(AbilitySystemComponent->GetSet<USoulAttributeSet>());

		if (ActiveAttributeSet)
		{
			// 플레이어가 쥐고 있는 변수 주소도 진짜 주소로 동기화해 줍니다.
			SoulAttributeSet = ActiveAttributeSet;

			UE_LOG(LogTemp, Warning, TEXT("ASoulCharacter::PossessedBy : 'AttributeSet - Die' Binding!"));
			SoulAttributeSet->OnHPChangedDelegate.AddUObject(this, &ASoulCharacter::ReturnDamageImpact); // 대미지를 받으면 실행하는 함수 바인딩
			SoulAttributeSet->OnHPZeroDelegate.AddUObject(this, &ASoulCharacter::CharacterDeath); // 캐릭터 HP == 0 에서 실행하는 함수 바인딩!
			
		}
	}
}

void ASoulCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	//AnimInstance
	USoulAnimInstance* AnimInstance = Cast<USoulAnimInstance>(GetMesh()->GetAnimInstance());

	AnimInstance->OnOnWeaponDelegate.AddUObject(this, &ASoulCharacter::OnWeapon);
	AnimInstance->OnOffWeaponDelegate.AddUObject(this, &ASoulCharacter::OffWeapon);

	//AnimInstance->OnAttackCheckDelegate.AddUObject(CombatComponent, &USoulCombatComponent::AttackCheck);

}

// Called every frame
void ASoulCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AbilitySystemComponent) //GA 제어.
	{
		AbilitySystemComponent->ProcessAbilityInput(DeltaTime, false);
	}
}

// Called to bind functionality to input
void ASoulCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (CharacterComponent)
	{
		CharacterComponent->InitializePlayerInput(InputComponent);
	}
}

void ASoulCharacter::OnControllerChanged(APawn* Pawn, AController* OldController, AController* NewController)
{
	APlayerController* PC = Cast<APlayerController>(NewController);
	if (PC) 
	{
		ULocalPlayer* LocalPlayer = PC->GetLocalPlayer();

		if (LocalPlayer) 
		{

			//Subsystem  "EnhancedInputSubsystems.h"
			UEnhancedInputLocalPlayerSubsystem* InputSubsystem =
				LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();

			if (InputSubsystem) 
			{
				InputSubsystem->AddMappingContext(InputMappingContext, 0);
			}
		}

	}
}

void ASoulCharacter::GiveAbilitySet()
{
	if (AbilitySet && AbilitySystemComponent)
	{
		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandles, this);
	}
}

	

void ASoulCharacter::WeaponGeneration()
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
}

void ASoulCharacter::OnWeapon()
{
	usesword->SetActorHiddenInGame(false);
	savesword->SetActorHiddenInGame(true);
}

void ASoulCharacter::OffWeapon()
{
	usesword->SetActorHiddenInGame(true);
	savesword->SetActorHiddenInGame(false);
}

void ASoulCharacter::OnGrabed()
{

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if(IsValid(AnimInstance)) 
	{
		//AnimInstance->Montage_Stop(0.f); 
		
		USoulAnimInstance* SoulAnim = Cast<USoulAnimInstance>(AnimInstance);

		SoulAnim->PlayStun();
	}

	AbilitySystemComponent->AddLooseGameplayTag(SoulGameplayTags::Gameplay_Rule_Stun, true);
	
	// ??? ????
	//GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();

	// ??? ????
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//(????)??? ????
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->DisableInput(PC);
	}

	// 3?? ?? ????
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, this, &ASoulCharacter::OnReleased, 3.0f, false);
}


void ASoulCharacter::OnReleased()
{

	GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->EnableInput(PC);
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		AnimInstance->Montage_Stop(0.f);
	}

	AbilitySystemComponent->RemoveLooseGameplayTag(SoulGameplayTags::Gameplay_Rule_Stun);

	if (AbilitySystemComponent->GetNumericAttribute(USoulAttributeSet::GetHPAttribute()) <= 0)
	{
		CombatComponent->TryGA_Death();
	}
}

void ASoulCharacter::FindEnemy(AActor* FindEnemy)
{
	this->Enemy = FindEnemy;
}

void ASoulCharacter::ReturnDamageImpact_Implementation()
{
	// 기존 카메라 셰이크 로직
	if (PlayerCameraComponent)
	{
		PlayerCameraComponent->CameraShaking_Damage();
	}
}

void ASoulCharacter::CharacterDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("ASoulCharacter::CharacterDeath"));

	// 1. [핵심] 이미 죽어가는 중이라면 아래 로직을 전부 무시하고 리턴!
	if (bIsDead)
	{
		return;
	}


	// 2. 들어오자마자 플래그를 true로 바꿔서 다음 프레임의 중복 호출을 차단
	bIsDead = true;

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->EnableInput(PC);
	}

	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (IsValid(AnimInstance))
	{
		AnimInstance->Montage_Stop(0.f);
	}

	CombatComponent->TryGA_Death();
}


