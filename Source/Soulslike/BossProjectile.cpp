// Fill out your copyright notice in the Description page of Project Settings.


#include "BossProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ABossProjectile::ABossProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));

	RootComponent = Mesh;

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->SetUpdatedComponent(Mesh);
	ProjectileMovement->InitialSpeed = Speed;
	ProjectileMovement->MaxSpeed = Speed;

	ProjectileMovement->bRotationFollowsVelocity = false; //움직이는 방향으로 회전할지 말지 설정


	//이동과 임펙트를 위한 타이머
	FollowTimer = 0.f;
	ImpactTimer = 0.f;
	OnFollow = true;
}

// Called when the game starts or when spawned
void ABossProjectile::BeginPlay()
{
	Super::BeginPlay();

	SetLifeSpan(3.0f);
}

// Called every frame
void ABossProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//임펙트/ 이동 타이머 계산
	MagicImpactSpawn(DeltaTime);
	MagicRInterpToMove(DeltaTime);
	
}

void ABossProjectile::MagicImpactSpawn(float DeltaTime)
{
	ImpactTimer += DeltaTime;

	if (ImpactTimer >= 0.1f)
	{
		ImpactTimer = 0.f;

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = GetInstigator();

		FTransform SpawnTransform;
		SpawnTransform.SetLocation(GetActorLocation() - GetActorForwardVector() * 20.f + GetActorUpVector() * 50.f);

		FRotator SpawnRotation = FRotator::ZeroRotator;

		SpawnTransform.SetScale3D(FVector(1.f));



		SpawnParams.SpawnCollisionHandlingOverride =
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AActor* MagicImpact = GetWorld()->SpawnActor<AActor>(
			SpawnActorClass,
			SpawnTransform,
			SpawnParams
		); //-> SetOwer(this);

		if (!MagicImpact)
		{
			UE_LOG(LogTemp, Log, TEXT("Bullet-Invalid"));
		}
	}
}


void ABossProjectile::MagicRInterpToMove(float DeltaTime)
{
	if (FollowTimer < 1.5)
	{
		FollowTimer += DeltaTime;
	}
	else
	{
		OnFollow = false;
	}

	if (OnFollow && IsValid(TargetCharacter)) //CPP 자체는 APawn과 ACharacter를 자동 참조하지 않는다.
	{
		FVector DreVector = (TargetCharacter->GetActorLocation() - GetActorLocation()).GetSafeNormal(); //단위벡터로 변경

		float InDot = FVector::DotProduct(DreVector, GetActorForwardVector());
		if (InDot > 0.3)
		{
			FVector Start = GetActorLocation();
			FVector Target = TargetCharacter->GetActorLocation();
			FRotator TargetRot = UKismetMathLibrary::FindLookAtRotation(Start, Target);
			//FindLookAtRotation : 두 벡터의 각을 구한다.

			FRotator NewRot = FMath::RInterpTo(
				GetActorRotation(), // 현재
				TargetRot,          // 목표 각
				DeltaTime,
				3.f                 // 회전 속도
			);

			SetActorRotation(NewRot);
			ProjectileMovement->Velocity = GetActorForwardVector() * ProjectileMovement->InitialSpeed;
		}
		else
		{
			OnFollow = false;
		}
	}
}

