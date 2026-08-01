// Fill out your copyright notice in the Description page of Project Settings.


#include "ClickMoveCharacter.h"
#include "Components/CapsuleComponent.h" //캡슐컴포넌트 정보를 가져온다.
//카메라와 스프링암 정보를 가져오는 헤더
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

// Sets default values
AClickMoveCharacter::AClickMoveCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	//이동시 이동하는 방향 바라보는 설정
	UCharacterMovementComponent* MovementComponent = GetCharacterMovement();
	MovementComponent->bOrientRotationToMovement = true; //가는 방향 바라보세요!
	MovementComponent->RotationRate = FRotator(0.f, 600.f, 0.f); //회전 속도 지정
	MovementComponent->bConstrainToPlane = true; //평지에서 동작하는것으로 Z축을 고정해준다.
	//쉽게 말해서 계단을 오르려 하면 못올라가게 막아버린다. 평면에서 동작시 고정 기능
	MovementComponent->bSnapToPlaneAtStart = true; //역시 평지 고정 기능인데,
	//bSnapToPlaneAtStart는 평면(평지)를 벗어났을때 고정해주는 기능이다.

	//카메라와 스프링암 컴포넌트를 추가해서 연결한다.
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraSpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->TargetArmLength = 800.f;
	SpringArmComponent->SetRelativeRotation(FRotator(-60.f, 45.0f, 0.0f));
	//스프링암의 위치와 회전등을 설정했다.

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;
	//카메라 회전시 캐릭터 회전을 막는다.


	

}

// Called when the game starts or when spawned
void AClickMoveCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AClickMoveCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AClickMoveCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

